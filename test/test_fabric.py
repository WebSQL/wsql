"""
Tests Connection Pools
"""
__author__ = "@bg"

try:
    from _case import DatabaseTestCase
    from _websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase
except ImportError:  # pragma: no cover
    from ._case import DatabaseTestCase
    from ._websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase

from unittest import TestCase
from websql.fabric import ConnectionPool, Upstream, transaction, retryable, Cluster, smart_connect
from websql.fabric.upstream import ServerInfo, Connection
from websql.fabric import exception


class DummyLogger:
    @staticmethod
    def error(msg, *args, **kwargs):
        pass


class TestServerInfo(TestCase):
    def test_to_str(self):
        srv_info = ServerInfo(host='localhost', port=3306)
        self.assertEqual('localhost:3306', str(srv_info))
        srv_info = ServerInfo(socket_name='/var/tmp/socket.sock')
        self.assertEqual('/var/tmp/socket.sock', str(srv_info))


class TestFabric(DatabaseTestCase):
    def make_upstream(self, servers):  # pragma: no cover
        """abstract method to create a upstream"""
        raise NotImplementedError

    def make_pool(self, upstream, timeout):  # pragma: no cover
        """abstract method to create connection pool"""
        raise NotImplementedError

    def get_insert_request(self, table, error=None):  # pragma: no cover
        """
        get the query request
        :param table: the temporary table name
        :param error: error, that should be raised if specified
        :return: the request, that run query logic
        """
        raise NotImplementedError

    def wrap_request(self, request):  # pragma: no cover
        """
        make a new request around specified
        :param request: the request to wrap
        :return: wrapped request
        """
        raise NotImplementedError

    def make_exception_request(self, retry_count):  # pragma: no cover
        """create request with retry_count"""
        raise NotImplementedError

    def count_calls(self, func):
        def wrapper(*args, **kwargs):
            wrapper.call_count += 1
            return func(*args, **kwargs)

        wrapper.call_count = 0
        return wrapper

    def test_next(self):
        """test next method of Upstream"""
        kwargs2 = self._context.connect_kwargs.copy()
        kwargs2['port'] = 1
        upstream = self.make_upstream([kwargs2, self._context.connect_kwargs])
        connection = self._context.wait(next(upstream))
        connection2 = self._context.wait(next(upstream))
        self.assertIs(self._context.connect_kwargs['host'], connection.meta.kwargs['host'])
        self.assertIs(self._context.connect_kwargs['host'], connection2.meta.kwargs['host'])
        self.assertTrue((x.penalty > 0 for x in upstream._servers))

    def test_no_connections(self):
        """test case when there is no online servers more"""
        kwargs2 = self._context.connect_kwargs.copy()
        kwargs2['port'] = 1
        upstream = self.make_upstream([kwargs2])
        self.assertRaises(RuntimeError, lambda: self._context.wait(next(upstream)))

    def test_invalidate(self):
        """test invalidate method of Upstream"""
        upstream = self.make_upstream([self._context.connect_kwargs])
        connection = self._context.wait(next(upstream))
        upstream.invalidate(connection)
        self.assertGreater(connection.meta.penalty, 0)
        connection.meta.penalty = 0
        upstream.invalidate(connection.meta)
        self.assertGreater(connection.meta.penalty, 0)
        self.assertGreater(upstream._servers[0].penalty, 0)
        self.assertRaises(ValueError, upstream.invalidate, 1)

    def test_acquire(self):
        """test _acquire method of ConnectionPoolAsync"""
        connections = []
        pool_size = 3
        timeout = 0.1

        pool = self.make_pool(self.make_upstream([{'count': pool_size}]), timeout)

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

        pool = self.make_pool(self.make_upstream([{"count": pool_size}]), timeout)
        connection = self._context.wait(pool._acquire())
        self.assertIsNotNone(connection)
        self.assertEqual(0, pool._queue.qsize())
        pool._release(connection)
        self.assertEqual(1, pool._queue.qsize())

    def test_release_closed_connection(self):
        """test _release method of ConnectionPoolAsync in case if connection closed"""
        pool_size = 3
        timeout = 0.1

        pool = self.make_pool(self.make_upstream([{"count": pool_size}]), timeout)
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

        pool = self.make_pool(self.make_upstream([{"count": pool_size}]), timeout)

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
        pool = self.make_pool(self.make_upstream([{"count": pool_size}]), timeout)
        connection = self._context.wait(pool.execute(self._context.decorator(lambda x: x)))
        self.assertIsNotNone(connection)

    def test_transaction_rollback(self):
        """test transaction rollback"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = Connection(self._context.make_connection())
        request = self.get_insert_request(table, Exception('rollback expected!'))
        self.assertRaisesRegex(Exception, 'rollback expected!',
                               self._context.call_and_wait, connection_holder.execute, transaction(request))

        cursor = self._context.cursor()
        self._context.wait(cursor.execute("SELECT * FROM %s" % table))
        self.assertEqual([], cursor.fetchall())

    def test_commit(self):
        """test transaction commit"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection = Connection(self._context.make_connection())
        request = self.get_insert_request(table)
        self.assertIs(self._context.wait(connection.execute(transaction(request))), True)

        self._context.wait(connection.rollback())
        cursor = self._context.cursor()
        self._context.wait(cursor.execute("SELECT * FROM %s" % table))
        self.assertEqual([('Evelina',)], cursor.fetchall())

    def test_recursive_transaction(self):
        """test recursive transaction"""
        table = self._create_table(('name VARCHAR(10)',), None)

        connection_holder = Connection(self._context.make_connection())
        request = self.wrap_request(transaction(self.get_insert_request(table)))

        connection_holder.commit = self.count_calls(connection_holder.commit)
        connection_holder.commit.call_count = 0
        self._context.wait(connection_holder.execute(transaction(request)))
        self.assertEqual(1, connection_holder.commit.call_count)

    def test_retries(self):
        """test retries"""
        retry_count = 3
        delay = 0.1

        connection = retryable(Connection(self._context.make_connection()), retry_count, delay)
        request = self.make_exception_request(retry_count - 1)
        self._context.wait(connection.execute(request))

        self.assertEqual(retry_count - 1, request.call_count)
        request = self.make_exception_request(retry_count + 1)
        self.assertRaises(Exception, self._context.call_and_wait, connection.execute, request)

    def test_cluster(self):
        """ test commutator """
        read_connection = Connection(self._context.make_connection())
        write_connection = Connection(self._context.make_connection())
        read_connection.readonly = True
        write_connection.readonly = False
        read_request = self.wrap_request(lambda x: self.assertTrue(x.readonly))
        write_request = transaction(self.wrap_request(lambda x: self.assertFalse(x.readonly)))

        cluster = Cluster(master=write_connection, slave=read_connection)
        self._context.wait(cluster.execute(read_request))
        self._context.wait(cluster.execute(write_request))

        cluster = Cluster(master=None, slave=read_connection)
        self.assertRaisesRegex(Exception, "the operation is not permitted on read-only cluster", cluster.execute, write_request)


class TestFabricSync(TestFabric):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetup())

    def make_upstream(self, servers):
        """create upstream"""
        return Upstream(servers, DummyLogger, nonblocking=False, **self._context.connect_kwargs)

    def make_pool(self, upstream, timeout):
        """create connection pool"""
        return ConnectionPool(upstream, timeout=timeout, nonblocking=False)

    def get_insert_request(self, table, error=None):
        def request(connection):
            cursor = self._context.wrap(connection.cursor())
            try:
                cursor.execute('INSERT INTO `%s` VALUES(\'%s\');' % (table, 'Evelina'))
            finally:
                cursor.close()

            if error:
                raise error
            return True

        return request

    def wrap_request(self, request):
        """
        make a new request around specified
        :param request: the request to wrap
        :return: wrapped request
        """
        return request

    def make_exception_request(self, retry_count):
        def request(_):
            request.call_count += 1
            if request.call_count < retry_count:
                exc = self._context.errors.Error(self._context.constants.CR_SERVER_LOST, "connection lost")
                exc.code = self._context.constants.CR_SERVER_LOST
                raise exc
            return None
        request.call_count = 0
        return request


class TestFabricAsync(TestFabric):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetupAsync())

    def make_upstream(self, servers):
        """create upstream"""
        return Upstream(servers, DummyLogger, loop=self._context.loop, nonblocking=True, **self._context.connect_kwargs)

    def make_pool(self, upstream, timeout):
        """create connection pool"""
        return ConnectionPool(upstream, timeout=timeout, loop=self._context.loop, nonblocking=True)

    def get_insert_request(self, table, error=None):
        @self._context.decorator
        def request(connection):
            cursor = connection.cursor()
            try:
                yield from cursor.execute('INSERT INTO %s VALUES (\'%s\')' % (table, 'Evelina'))
            finally:
                yield from cursor.close()

            if error:
                raise error
            return True

        return request

    def wrap_request(self, request):
        """
        make a new request around specified
        :param request: the request to wrap
        :return: wrapped request
        """
        @self._context.decorator
        def wrapper(connection):
            if self._context.iscoroutine(request):
                return (yield from request(connection))
            return request(connection)
        return wrapper

    def make_exception_request(self, retry_count):
        @self._context.decorator
        def request(_):
            request.call_count += 1
            if request.call_count < retry_count:
                exc = self._context.errors.Error(self._context.constants.CR_SERVER_LOST, "connection lost")
                exc.code = self._context.constants.CR_SERVER_LOST
                raise exc
            return None
        request.call_count = 0
        return request

    def test_commit(self):
        super().test_commit()

del TestFabric


class TestException(TestCase):
    """
    test user exceptions
    """
    class TestError(exception.UserError):
        pass

    def test_handle_error(self):
        """test handle_error method"""

        self.assertRaisesRegex(self.TestError, "(1, 'this is test error')", exception.handle_error, self, exception.UserError(1, "TestError; this is test error"))
        self.assertRaisesRegex(exception.UserError, "(1, 'Test2Error; this is test error')", exception.handle_error, self, exception.UserError(1, "Test2Error; this is test error"))
        self.assertRaisesRegex(exception.UserError, "(1, 'this is test error')", exception.handle_error, self, exception.UserError(1, "this is test error"))
        self.assertRaisesRegex(ValueError, "this is test error", exception.handle_error, self, ValueError("this is test error"))


class TestSmartConnect(TestCase):
    def test_smart_connect(self):
        """test construct smart connect"""
        connection_args = {"master": "localhost:3306#2,localhost#4", "slave": "localhost:3306#2", "database": "test"}
        connection = smart_connect(connection_args, nonblocking=False)
        self.assertIsInstance(connection, Cluster)
        self.assertEqual(6, len(connection._cluster[1]._connection._upstream))
        self.assertEqual(2, len(connection._cluster[0]._connection._upstream))
        connection_args = {"slave": "localhost:3306#2", "database": "test"}
        self.assertIsInstance(smart_connect(connection_args, nonblocking=False), Cluster)
        connection_args = {"master": "localhost:3306#2,localhost#4", "database": "test"}
        self.assertFalse(isinstance(smart_connect(connection_args, nonblocking=False), Cluster))
        connection_args = {}
        self.assertFalse(isinstance(smart_connect(connection_args, nonblocking=False), Cluster))
        connection_args = "master=localhost:3306#2,localhost#4;slave=localhost:3306#2;database=test;"
        connection = smart_connect(connection_args, nonblocking=False)
        self.assertIsInstance(connection, Cluster)
        self.assertEqual(6, len(connection._cluster[1]._connection._upstream))
        self.assertEqual(2, len(connection._cluster[0]._connection._upstream))
