"""
Tests Connection Pools
"""
__author__ = "@bg"

try:
    from _case import DatabaseTestCase
    from _websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase
except ImportError:
    from ._case import DatabaseTestCase
    from ._websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase

from unittest import TestCase
from websql.fabric import ConnectionPool, ConnectionProvider, transaction, retryable
from websql.fabric.provider import ServerInfo, ConnectionHolderAsync, ConnectionHolderSync


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


class TestFabric(DatabaseTestCase):
    def make_provider(self, servers):
        """abstract method to create a new provider"""
        raise NotImplementedError

    def make_pool(self, provider, size, timeout):
        """abstract method to create connection pool"""
        raise NotImplementedError

    def get_insert_handler(self, table, error=None):
        """
        get the query handler
        :param table: the temporary table name
        :param error: error, that should be raised if specified
        :return: the handler, that run query logic
        """
        raise NotImplementedError

    def make_connection_holder(self, connection):
        """
        Create a new connection holder
        :param connection: real connection to database
        :return: the ConnectionHolder
        """
        raise NotImplementedError

    def wrap_handler(self, handler):
        """
        make a new handler around specified
        :param handler: the handler to wrap
        :return: wrapped handler
        """
        raise NotImplementedError

    def make_retryable(self, connection, retry_count, delay):
        """create a new retryable connection to database"""
        raise NotImplementedError

    def make_exception_handler(self, retry_count):
        """create handler with retry_count"""
        raise NotImplementedError

    def count_calls(self, func):
        def wrapper(*args, **kwargs):
            wrapper.call_count += 1
            return func(*args, **kwargs)

        wrapper.call_count = 0
        return wrapper

    def test_connect(self):
        """test connect method of ConnectionProvider"""
        kwargs2 = self._context.connect_kwargs.copy()
        kwargs2['port'] = 1
        provider = self.make_provider([kwargs2, self._context.connect_kwargs])
        connection = self._context.wait(provider.connect())
        connection2 = self._context.wait(provider.connect())
        self.assertIs(self._context.connect_kwargs, connection.meta.kwargs)
        self.assertIs(self._context.connect_kwargs, connection2.meta.kwargs)
        self.assertGreater(provider._servers[0].penalty, 0)

    def test_no_connections(self):
        """test case when there is no online servers more"""
        kwargs2 = self._context.connect_kwargs.copy()
        kwargs2['port'] = 1
        provider = self.make_provider([kwargs2])
        self.assertRaises(RuntimeError, lambda: self._context.wait(provider.connect()))

    def test_invalidate(self):
        """test invalidate method of ConnectionProvider"""
        provider = self.make_provider([self._context.connect_kwargs])
        connection = self._context.wait(provider.connect())
        provider.invalidate(connection)
        self.assertGreater(connection.meta.penalty, 0)
        connection.meta.penalty = 0
        provider.invalidate(connection.meta)
        self.assertGreater(connection.meta.penalty, 0)
        self.assertGreater(provider._servers[0].penalty, 0)
        self.assertRaises(ValueError, provider.invalidate, 1)

    def test_acquire(self):
        """test _acquire method of ConnectionPoolAsync"""
        connections = []
        pool_size = 3
        timeout = 0.1

        pool = self.make_pool(self.make_provider([self._context.connect_kwargs]), pool_size, timeout)

        for i in range(pool_size):
            connection = self._context.wait(pool._acquire())
            self.assertIsNotNone(connection)
            connections.append(connection)
            self.assertEqual(pool_size - i - 1, pool._reserve)
        self.assertEqual(pool_size, len(connections))

    def test_release(self):
        """test _release method of ConnectionPoolAsync"""
        pool_size = 3
        timeout = 0.1

        pool = self.make_pool(self.make_provider([self._context.connect_kwargs]), pool_size, timeout)
        connection = self._context.wait(pool._acquire())
        self.assertIsNotNone(connection)
        self.assertEqual(0, pool._queue.qsize())
        pool._release(connection)
        self.assertEqual(1, pool._queue.qsize())

    def test_release_closed_connection(self):
        """test _release method of ConnectionPoolAsync in case if connection closed"""
        pool_size = 3
        timeout = 0.1

        pool = self.make_pool(self.make_provider([self._context.connect_kwargs]), pool_size, timeout)
        connection = self._context.wait(pool._acquire())
        self.assertIsNotNone(connection)
        self.assertEqual(0, pool._queue.qsize())
        connection.connection().close()
        pool._release(connection)
        self.assertEqual(0, pool._queue.qsize())
        self.assertEqual(pool_size, pool._reserve)

    def test_acquire_if_no_free(self):
        """test _acquire method of ConnectionPoolAsync if there is no free connections"""
        connections = []
        pool_size = 3
        timeout = 0.1

        pool = self.make_pool(self.make_provider([self._context.connect_kwargs]), pool_size, timeout)

        for i in range(pool_size):
            connection = self._context.wait(pool._acquire())
            self.assertIsNotNone(connection)
            connections.append(connection)
            self.assertEqual(pool_size - i - 1, pool._reserve)
        self.assertEqual(0, pool._reserve)
        self.assertTrue(pool._queue.empty())
        self.assertRaises(pool.TimeoutError, self._context.call_and_wait, pool._acquire)

    def test_connection_pool_execute(self):
        """test _release method of ConnectionPoolAsync"""
        pool_size = 3
        timeout = 0.1
        pool = self.make_pool(self.make_provider([self._context.connect_kwargs]), pool_size, timeout)
        connection = self._context.wait(pool.execute(self._context.decorator(lambda x: x)))
        self.assertIsNotNone(connection)

    def test_transaction_rollback(self):
        """test transaction rollback"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = self.make_connection_holder(self._context.make_connection())
        handler = self.get_insert_handler(table, Exception('rollback expected!'))
        self.assertRaisesRegex(Exception, 'rollback expected!',
                               self._context.call_and_wait, connection_holder.execute, transaction(handler))

        cursor = self._context.cursor()
        self._context.wait(cursor.execute("SELECT * FROM %s" % table))
        self.assertEqual([], cursor.fetchall())

    def test_commit(self):
        """test transaction commit"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = self.make_connection_holder(self._context.make_connection())
        handler = self.get_insert_handler(table)
        self._context.wait(connection_holder.execute(transaction(handler)))

        cursor = self._context.cursor()
        self._context.wait(cursor.execute("SELECT * FROM %s" % table))
        self.assertEqual([('Evelina',)], cursor.fetchall())

    def test_recursive(self):
        """test recursive transaction"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = self.make_connection_holder(self._context.make_connection())
        handler = self.wrap_handler(transaction(self.get_insert_handler(table)))

        connection_holder.commit = self.count_calls(connection_holder.commit)
        connection_holder.commit.call_count = 0
        self._context.wait(connection_holder.execute(transaction(handler)))
        self.assertEqual(1, connection_holder.commit.call_count)

    def test_retries(self):
        """test retries"""
        retry_count = 3
        delay = 0.1

        connection = self.make_retryable(self.make_connection_holder(self._context.make_connection()), retry_count, delay)
        handler = self.make_exception_handler(retry_count - 1)
        self._context.wait(connection.execute(handler))

        self.assertEqual(retry_count - 1, handler.call_count)
        handler = self.make_exception_handler(retry_count + 1)
        self.assertRaises(Exception, self._context.call_and_wait, connection.execute, handler)


class TestFabricSync(TestFabric):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetup())

    def make_provider(self, servers):
        """create connection provider"""
        return ConnectionProvider(servers, DummyLogger, nonblocking=False)

    def make_pool(self, provider, size, timeout):
        """create connection pool"""
        return ConnectionPool(provider, size, timeout=timeout, nonblocking=False)

    def get_insert_handler(self, table, error=None):
        def handler(connection):
            cursor = self._context.wrap(connection.cursor())
            try:
                self._context.wait(cursor.execute('INSERT INTO `%s` VALUES(\'%s\');' % (table, 'Evelina')))
            finally:
                cursor.close()

            if error:
                raise error

        return handler

    def make_connection_holder(self, connection):
        """
        Create a new connection holder
        :param connection: real connection to database
        :return: the ConnectionHolder
        """
        return ConnectionHolderSync(connection)

    def wrap_handler(self, handler):
        """
        make a new handler around specified
        :param handler: the handler to wrap
        :return: wrapped handler
        """
        def wrapper(connection):
            return handler(connection)
        return wrapper

    def make_exception_handler(self, retry_count):
        def handler(_):
            handler.call_count += 1
            if handler.call_count < retry_count:
                exc = self._context.errors.Error(self._context.constants.CR_SERVER_LOST, "connection lost")
                exc.code = self._context.constants.CR_SERVER_LOST
                raise exc
            return None
        handler.call_count = 0
        return handler

    def make_retryable(self, connection, retry_count, delay):
        return retryable(connection, retry_count, delay)


class TestFabricAsync(TestFabric):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetupAsync())

    def make_provider(self, servers):
        """create connection provider"""
        return ConnectionProvider(servers, DummyLogger, loop=self._context.loop, nonblocking=True)

    def make_pool(self, provider, size, timeout):
        """create connection pool"""
        return ConnectionPool(provider, size, timeout=timeout, loop=self._context.loop, nonblocking=True)

    def get_insert_handler(self, table, error=None):
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

    def make_connection_holder(self, connection):
        """
        Make connection holder
        :param connection: real connection to database
        :return: the ConnectionHolder
        """
        return ConnectionHolderAsync(connection)

    def make_exception_handler(self, retry_count):
        @self._context.decorator
        def handler(_):
            handler.call_count += 1
            if handler.call_count < retry_count:
                exc = self._context.errors.Error(self._context.constants.CR_SERVER_LOST, "connection lost")
                exc.code = self._context.constants.CR_SERVER_LOST
                raise exc
            return None
        handler.call_count = 0
        return handler

    def make_retryable(self, connection, retry_count, delay):
        return retryable(connection, retry_count, delay, loop=self._context.loop)

del TestFabric
