__author__ = "@bg"

from asyncio import coroutine, wait_for
from time import monotonic
import websql


class ServerInfo:
    def __init__(self, kwargs):
        self.kwargs = kwargs
        self.penalty = 0

    def __str__(self):
        try:
            return ':'.join((self.kwargs['host'], str(self.kwargs.get('port', '3306'))))
        except KeyError:
            return self.kwargs.get('socket_name', 'default')


class ConnectionProviderBase:
    """
    Base class for connection providers
    """
    def __init__(self, servers, logger):
        """
        Constructor
        :param servers: the list of ServerInfo objects
        :param logger: the logger object
        """
        self._servers = [ServerInfo(x) for x in servers]
        self._logger = logger
        self.current = 0
        self.max = len(servers)
        self.penalty = 60  # 1 min

    @staticmethod
    def attach_info(connection, info):
        """
        Attach meta info to connection
        :param connection: the connection object
        :param info: the server info
        :return: the connection object
        """
        connection.meta_info = info
        return connection

    def invalidate(self, connection):
        """
        Invalidate the disconnected connection
        the appropriate server will be banned to ban timeout
        :param connection: the connection object, that contains meta information
        """

        if not isinstance(connection, ServerInfo):
            connection = connection.meta_info
        connection.penalty = monotonic() + self.penalty
        self._logger.error('Connection to server %s closed.', connection)


class ConnectionProviderAsync(ConnectionProviderBase):
    """
    The asynchronous variant of ConnectionProvider
    """
    def __init__(self, servers, logger, loop=None):
        """
        Constructor
        :param servers: the list of ServerInfo objects
        :param logger: the logger object
        :param loop: the event loop
        """
        super().__init__(servers, logger)
        self._loop = loop

    @coroutine
    def connect(self):
        """
        Create a new connection to database, try one by one all servers.
        if there is no online servers the RuntimeError will be raised
        :return: the connection object
        """
        current = self.current
        time_ = monotonic()
        for idx in ((i + current) % self.max for i in range(self.max)):
            info = self._servers[idx]
            if info.penalty < time_:
                try:
                    return self.attach_info((yield from websql.connect(nonblocking=True, loop=self._loop, **info.kwargs)), info)
                except websql.Error:
                    self.invalidate(info)

        raise RuntimeError('There is no online servers.')


class ConnectionProvider(ConnectionProviderBase):
    """
    The synchronous variant of ConnectionProvider
    """

    def connect(self):
        """
        Create a new connection to database, try one by one all servers.
        if there is no online servers the RuntimeError will be raised
        :return: the connection object
        """

        current = self.current
        time_ = monotonic()
        for idx in ((i + current) % self.max for i in range(self.max)):
            info = self._servers[idx]
            if info.penalty < time_:
                try:
                    return self.attach_info(websql.connect(nonblocking=False, **info.kwargs), info)
                except websql.Error:
                    self.invalidate(info)

        raise RuntimeError('There is no online servers.')


class ConnectionPoolBase:
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
        if connection.closed:
            self._reserve += 1
            self._provider.invalidate(connection)
        else:
            self._queue.put_nowait(connection)


class ConnectionPoolAsync(ConnectionPoolBase):
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
            return handler(connection)
        finally:
            self._release(connection)
