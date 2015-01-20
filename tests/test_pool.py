"""
Tests Connection Pools
"""
__author__ = "@bg"

from tests.websql_context import WebSQLSetupAsync, WebSQLSetup
from unittest import TestCase
from websql.pool import ConnectionPool, ConnectionPoolAsync


class TestPollBase(TestCase):
    pool_size = 3
    timeout = 1
    setup = None
    pool = None

    @classmethod
    def tearDownClass(cls):
        cls.setup.clean()
        cls.setup = None

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
        connection.close()
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
        self.pool = ConnectionPool(self.setup.connect, self.pool_size, timeout=self.timeout)

    def tearDown(self):
        self.pool = None


class TestPoolAsync(TestPollBase):
    @classmethod
    def setUpClass(cls):
        cls.setup = WebSQLSetupAsync()

    def setUp(self):
        self.pool = ConnectionPoolAsync(self.setup.connect, self.pool_size, timeout=self.timeout, loop=self.setup.loop)

    def tearDown(self):
        self.pool = None

del TestPollBase
