"""
WebSQL Connection Pool
----------------------

This module implements connections pools for WebSQL.
"""

from .functional import nonblocking as _nonblocking
from asyncio import coroutine, wait_for


__all__ = ["ConnectionPool"]


def connection_poll(*args, nonblocking=True, **kwargs):
    """
    create a new connection pool
    :param args: connection pool positional arguments
    :param nonblocking: if True, the non-blocking version will be created
    :param kwargs: connection pool keyword arguments
    :return: the new ConnectionPool
    """

    if nonblocking:
        return ConnectionPoolAsync(*args, **kwargs)
    return ConnectionPoolSync(*args, **kwargs)

ConnectionPool = connection_poll


class _ConnectionPoolBase:
    """
        Base class for connection pools
    """

    def __init__(self, provider, queue, timeout=None):
        """
        Constructor
        :param provider: factory to create a new connection
        :type provider: ConnectionProvider
        :param queue: the queue to store connections
        :type queue: Queue
        :param timeout: the timeout in seconds to acquire new connection
        :param timeout: int
        """
        assert queue.maxsize

        self._provider = provider
        self._queue = queue
        self._reserve = queue.maxsize
        self._timeout = timeout

    def _release(self, connection):
        """
        put back a connection to free connections queue if connection is valid
        :param connection: connection object
        """
        if connection.connected:
            self._queue.put_nowait(connection)
        else:
            self._reserve += 1
            self._provider.invalidate(connection)


@_nonblocking
class ConnectionPoolAsync(_ConnectionPoolBase):
    """
        Asynchronous connection pool
        the connections will be created on demand
    """

    def __init__(self, provider, size, loop=None, timeout=None):
        """
        Initialize pool
        :param provider: coroutine to create a new connection
        :param size: the pool size
        :param loop: the event loop, by default current event loop will be used
        """
        from asyncio.queues import Queue, QueueEmpty
        from asyncio import TimeoutError, get_event_loop

        super().__init__(provider, Queue(size, loop=loop), timeout=timeout)
        self._loop = loop or get_event_loop()
        self.QueueEmpty = QueueEmpty
        self.TimeoutError = TimeoutError

    @coroutine
    def _acquire(self):
        """
        acquire free connection from free connections queue or create a new one if possible
        :return: connection
        """
        try:
            return self._queue.get_nowait()
        except self.QueueEmpty:
            pass

        if self._reserve == 0:
            return (yield from wait_for(self._queue.get(), timeout=self._timeout, loop=self._loop))

        self._reserve -= 1
        return (yield from wait_for(self._provider.connect(), timeout=self._timeout, loop=self._loop))

    @coroutine
    def execute(self, handler):
        """
        execute the database query on connection pool
        :param handler: - the callable object, that implement logic to query database
        :return the result of handler
        """
        connection = yield from self._acquire()
        try:
            return (yield from connection.execute(handler))
        finally:
            self._release(connection)


class ConnectionPoolSync(_ConnectionPoolBase):
    """
        Synchronous connection pool
        the connections will be created on demand
    """

    TimeoutError = TimeoutError

    def __init__(self, provider, size, timeout=None):
        """
        Initialize pool
        :param provider: method to create a new connection
        :param size: the pool size
        :param timeout: the timeout to create a new connection
        """
        from queue import Queue, Empty
        from threading import Lock

        super().__init__(provider, Queue(size), timeout=timeout)
        self._lock = Lock()
        self.QueueEmpty = Empty

    def _acquire(self):
        """
        acquire free connection from free connections queue or create a new one if possible
        :return: connection
        """
        try:
            return self._queue.get_nowait()
        except self.QueueEmpty:
            pass

        can_create = False

        with self._lock:
            if self._reserve > 0:
                self._reserve -= 1
                can_create = True

        if can_create:
            return self._provider.connect()

        try:
            return self._queue.get(timeout=self._timeout)
        except self.QueueEmpty:
            raise self.TimeoutError from None

    def execute(self, handler):
        """
        execute the database query on connection pool
        :param handler: - the callable object, that implement logic to query database
        :return the result of handler
        """
        connection = self._acquire()
        try:
            return connection.execute(handler)
        finally:
            self._release(connection)
