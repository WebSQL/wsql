"""
Tests Connection Pools
"""
__author__ = "@bg"

from tests.websql_context import WebSQLSetupAsync, WebSQLSetup
from unittest import TestCase
from websql.fabric import ConnectionPool, ConnectionProvider
from websql.fabric.provider import ServerInfo


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
    provider_class = None

    @classmethod
    def tearDownClass(cls):
        cls.setup.clean()
        cls.setup = None

    def tearDown(self):
        self.pool = None

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
        connection.connection.close()
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
