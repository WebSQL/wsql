__author__ = "@bg"

from asyncio import coroutine, wait_for


class ConnectionPoolBase:
    """
        Base class for connection pools
    """
    def __init__(self, factory, queue, timeout=None):
        """
        Constructor
        :param factory: factory to create a new connection
        :param queue: the queue to store connections
        """
        self._factory = factory
        self._queue = queue
        self._timeout = timeout


class ConnectionPoolAsync(ConnectionPoolBase):
    """
        Asynchronous connection pool
        the connections will be created on demand
    """
    def __init__(self, factory, size, loop=None, timeout=None):
        """
        Initialize pool
        :param factory: coroutine to create a new connection
        :param size: the pool size
        :param loop: the event loop, by default current event loop will be used
        """
        from asyncio.queues import Queue, QueueEmpty
        from asyncio import TimeoutError, get_event_loop

        super().__init__(factory, Queue(size, loop=loop), timeout=timeout)
        self._reserve = size
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
        return (yield from wait_for(self._factory(), timeout=self._timeout, loop=self._loop))

    def _release(self, connection):
        """
        put back a connection to free connections queue if connection is valid
        :param connection: connection object
        """
        if connection.closed:
            self._reserve += 1
        else:
            self._queue.put_nowait(connection)

    @coroutine
    def execute(self, handler):
        """
        execute the database query on connection pool
        :param handler: - the callable object, that implement logic to query database
        :return the result of handler
        """
        connection = yield from self._acquire()
        try:
            return handler(connection)
        finally:
            self._release(connection)


class ConnectionPool(ConnectionPoolBase):
    """
        Synchronous connection pool
        the connections will be created on demand
    """

    TimeoutError = TimeoutError

    def __init__(self, factory, size, timeout=None):
        """
        Initialize pool
        :param factory: method to create a new connection
        :param size: the pool size
        :param timeout: the timeout to create a new connection
        """
        from queue import Queue, Empty
        from threading import Lock

        super().__init__(factory, Queue(size), timeout=timeout)
        self._reserve = size
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
            return self._factory()

        try:
            return self._queue.get(timeout=self._timeout)
        except self.QueueEmpty:
            raise self.TimeoutError from None

    def _release(self, connection):
        """
        put back a connection to free connections queue if connection is valid
        :param connection: connection object
        """
        if connection.closed:
            self._reserve += 1
        else:
            self._queue.put_nowait(connection)

    def execute(self, handler):
        """
        execute the database query on connection pool
        :param handler: - the callable object, that implement logic to query database
        :return the result of handler
        """
        connection = self._acquire()
        try:
            return handler(connection)
        finally:
            self._release(connection)
