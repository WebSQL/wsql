"""
Tests Connection Pools
"""
__author__ = "@bg"

from tests.case import DatabaseTestCase
from tests.websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase
from unittest import TestCase
from websql.fabric import ConnectionPool, ConnectionProvider, transaction
from websql.fabric.provider import ServerInfo, ConnectionHolder


class DummyLogger:
    @staticmethod
    def error(msg, *args, **kwargs):
        pass


class TestServerInfo(TestCase):
    def test_to_str(self):
        srv_info = ServerInfo({'host': 'localhost', 'port': 3306})
        self.assertEqual('localhost:3306', str(srv_info))
        srv_info = ServerInfo({'socket_name': '/var/tmp/socket.sock'})
        self.assertEqual('/var/tmp/socket.sock', str(srv_info))


class TestProviderBase(TestCase):
    setup = None

    @classmethod
    def tearDownClass(cls):
        cls.setup.clean()
        cls.setup = None

    def test_connect(self):
        """test connect method of ConnectionProvider"""
        kwargs2 = self.setup.connect_kwargs.copy()
        kwargs2['port'] = 1
        provider = self.make_provider([kwargs2, self.setup.connect_kwargs])
        connection = self.setup.wait(provider.connect())
        connection2 = self.setup.wait(provider.connect())
        self.assertIs(self.setup.connect_kwargs, connection.meta.kwargs)
        self.assertIs(self.setup.connect_kwargs, connection2.meta.kwargs)
        self.assertGreater(provider._servers[0].penalty, 0)

    def test_no_connections(self):
        """test case when there is no online servers more"""
        kwargs2 = self.setup.connect_kwargs.copy()
        kwargs2['port'] = 1
        provider = self.make_provider([kwargs2])
        self.assertRaises(RuntimeError, lambda: self.setup.wait(provider.connect()))

    def test_invalidate(self):
        """test invalidate method of ConnectionProvider"""
        provider = self.make_provider([self.setup.connect_kwargs])
        connection = self.setup.wait(provider.connect())
        provider.invalidate(connection)
        self.assertGreater(connection.meta.penalty, 0)
        connection.meta.penalty = 0
        provider.invalidate(connection.meta)
        self.assertGreater(connection.meta.penalty, 0)
        self.assertGreater(provider._servers[0].penalty, 0)
        self.assertRaises(ValueError, provider.invalidate, 1)

    def make_provider(self, servers):
        """abstract method to create a new provider"""
        raise NotImplementedError


class TestProvider(TestProviderBase):
    @classmethod
    def setUpClass(cls):
        cls.setup = WebSQLSetup()

    def make_provider(self, servers):
        """abstract method to create a new provider"""
        return ConnectionProvider(servers, DummyLogger, nonblocking=False)


class TestProviderAsync(TestProviderBase):
    @classmethod
    def setUpClass(cls):
        cls.setup = WebSQLSetupAsync()

    def make_provider(self, servers):
        """abstract method to create a new provider"""
        return ConnectionProvider(servers, DummyLogger, loop=self.setup.loop, nonblocking=True)

del TestProviderBase


class TestPollBase(TestCase):
    pool_size = 3
    timeout = 1
    setup = None
    pool = None

    @classmethod
    def tearDownClass(cls):
        cls.setup.clean()
        cls.setup = None

    def tearDown(self):
        self.pool = None

    def test_acquire(self):
        """test _acquire method of ConnectionPoolAsync"""
        connections = []
        for i in range(self.pool_size):
            connection = self.setup.wait(self.pool._acquire())
            self.assertIsNotNone(connection)
            connections.append(connection)
            self.assertEqual(self.pool_size - i - 1, self.pool._reserve)
        self.assertEqual(self.pool_size, len(connections))

    def test_release(self):
        """test _release method of ConnectionPoolAsync"""
        connection = self.setup.wait(self.pool._acquire())
        self.assertIsNotNone(connection)
        self.assertEqual(0, self.pool._queue.qsize())
        self.pool._release(connection)
        self.assertEqual(1, self.pool._queue.qsize())

    def test_release_closed_connection(self):
        """test _release method of ConnectionPoolAsync in case if connection closed"""
        connection = self.setup.wait(self.pool._acquire())
        self.assertIsNotNone(connection)
        self.assertEqual(0, self.pool._queue.qsize())
        connection.connection().close()
        self.pool._release(connection)
        self.assertEqual(0, self.pool._queue.qsize())
        self.assertEqual(self.pool_size, self.pool._reserve)

    def test_acquire_if_no_free(self):
        """test _acquire method of ConnectionPoolAsync if there is no free connections"""
        connections = []
        for i in range(self.pool_size):
            connection = self.setup.wait(self.pool._acquire())
            self.assertIsNotNone(connection)
            connections.append(connection)
            self.assertEqual(self.pool_size - i - 1, self.pool._reserve)
        self.assertEqual(0, self.pool._reserve)
        self.assertTrue(self.pool._queue.empty())
        self.assertRaises(self.pool.TimeoutError, self.setup.call_and_wait, self.pool._acquire)

    def test_execute(self):
        """test _release method of ConnectionPoolAsync"""
        connection = self.setup.wait(self.pool.execute(lambda x: x))
        self.assertIsNotNone(connection)


class TestPool(TestPollBase):
    @classmethod
    def setUpClass(cls):
        cls.setup = WebSQLSetup()

    def setUp(self):
        provider = ConnectionProvider([self.setup.connect_kwargs], DummyLogger, nonblocking=False)
        self.pool = ConnectionPool(provider, self.pool_size, timeout=self.timeout, nonblocking=False)


class TestPoolAsync(TestPollBase):
    @classmethod
    def setUpClass(cls):
        cls.setup = WebSQLSetupAsync()

    def setUp(self):
        provider = ConnectionProvider([self.setup.connect_kwargs], DummyLogger, loop=self.setup.loop, nonblocking=True)
        self.pool = ConnectionPool(provider, self.pool_size, timeout=self.timeout, loop=self.setup.loop, nonblocking=True)


del TestPollBase


class TestTransactionScopeBase(DatabaseTestCase):
    def count_calls(self, func):
        def wrapper(*args, **kwargs):
            wrapper.call_count += 1
            return func(*args, **kwargs)

        wrapper.call_count = 0
        return wrapper

    def test_rollback(self):
        """test rollback method of TransactionScope"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = ConnectionHolder(self._context.make_connection())
        handler = self.get_hanlder(table, Exception('rollback expected!'))
        self.assertRaisesRegex(Exception, 'rollback expected!',
                               self._context.call_and_wait, connection_holder.execute, transaction(handler))

        cursor = self._context.cursor()
        self._context.wait(cursor.execute("SELECT * FROM %s" % table))
        self.assertEqual([], cursor.fetchall())

    def test_commit(self):
        """test commit method of TransactionScope"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = ConnectionHolder(self._context.make_connection())
        handler = self.get_hanlder(table)
        self._context.wait(connection_holder.execute(transaction(handler)))

        cursor = self._context.cursor()
        self._context.wait(cursor.execute("SELECT * FROM %s" % table))
        self.assertEqual([('Evelina',)], cursor.fetchall())

    def test_recursive(self):
        """test get recursive handler"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = ConnectionHolder(self._context.make_connection())
        handler = self.wrap_handler(transaction(self.get_hanlder(table)))

        connection_holder.commit = self.count_calls(connection_holder.commit)
        connection_holder.commit.call_count = 0
        self._context.wait(connection_holder.execute(transaction(handler)))
        self.assertEqual(1, connection_holder.commit.call_count)

    def get_hanlder(self, table, error=None):
        """
        get the query handler
        :param table: the temporary table name
        :param error: error, that should be raised if specified
        :return: the handler, that run query logic
        """
        raise NotImplementedError

    def wrap_handler(self, handler):
        """
        make a new handler around specified
        :param handler: the handler to wrap
        :return: wrapped handler
        """
        raise NotImplementedError


class TestTransactionScope(TestTransactionScopeBase):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetup())

    def get_hanlder(self, table, error=None):
        @self._context.decorator
        def handler(connection):
            cursor = self._context.wrap(connection.cursor())
            try:
                self._context.wait(cursor.execute('INSERT INTO `%s` VALUES(\'%s\');' % (table, 'Evelina')))
            finally:
                cursor.close()

            if error:
                raise error
        return handler

    def wrap_handler(self, handler):
        """
        make a new handler around specified
        :param handler: the handler to wrap
        :return: wrapped handler
        """
        def wrapper(connection):
            return handler(connection)
        return wrapper


class TestTransactionScopeAsync(TestTransactionScopeBase):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetupAsync())

    def get_hanlder(self, table, error=None):
        @self._context.decorator
        def handler(connection):
            cursor = connection.cursor()
            try:
                yield from cursor.execute('INSERT INTO %s VALUES (\'%s\')' % (table, 'Evelina'))
            finally:
                yield from cursor.close()

            if error:
                raise error
        return handler

    def wrap_handler(self, handler):
        """
        make a new handler around specified
        :param handler: the handler to wrap
        :return: wrapped handler
        """
        @self._context.decorator
        def wrapper(connection):
            return (yield from handler(connection))
        return wrapper


del TestTransactionScopeBase
