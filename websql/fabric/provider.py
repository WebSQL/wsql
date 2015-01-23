"""
WebSQL Connection Provider
-------------------

This module implements connections pools for WebSQL.
"""

from asyncio import coroutine
from time import monotonic
import websql
import sys

__all__ = ["ConnectionProvider"]


def connection_provider(*args, nonblocking=True, **kwargs):
    """
    create a new connection provider
    :param args: connection provider positional arguments
    :param nonblocking: if True, the non-blocking version will be created
    :param kwargs: connection provider keyword arguments
    :return: the new ConnectionProvider
    """
    if nonblocking:
        return ConnectionProviderAsync(*args, **kwargs)
    return ConnectionProviderSync(*args, **kwargs)

ConnectionProvider = connection_provider


class ServerInfo:
    def __init__(self, kwargs):
        """
        :param kwargs: native connection arguments
        """
        self.kwargs = kwargs
        self.penalty = 0

    def __str__(self):
        try:
            return ':'.join((self.kwargs['host'], str(self.kwargs.get('port', '3306'))))
        except KeyError:
            return self.kwargs.get('socket_name', 'default')


class ConnectionHolder:
    """connection holder"""
    def __init__(self, connection, meta=None):
        """
        :param connection: the native connection to database
        :param meta: the meta information associated with this connection
        """
        self._connection = connection
        self._meta = meta

    def connection(self):
        """
        :return: the native connection
        """
        return self._connection

    def cursor(self):
        """
        :return: the native cursor
        """
        return self._connection.cursor()

    @property
    def closed(self):
        """
        :return: the connection state
        """
        return self._connection.closed

    @property
    def meta(self):
        """
        :return: connection meta information
        """
        return self._meta

    def execute(self, handler):
        """
        execute the database query
        :param handler: - the callable object, that implement logic to query database
        :return the result of handler
        """
        return handler(self)

    def commit(self):
        """apply changes"""
        return self._connection.commit()

    def rollback(self):
        """rollback changes"""
        return self._connection.rollback()


class _ConnectionProviderBase:
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
    def make_connection(connection, info):
        """
        Attach meta info to connection
        :param connection: the connection object
        :param info: the server info
        :return: the connection object
        """
        return ConnectionHolder(connection, info)

    def invalidate(self, connection, e=None):
        """
        Invalidate the disconnected connection
        the appropriate server will be banned to ban timeout
        :param connection: the connection object, that contains meta information
        """

        if isinstance(connection, ConnectionHolder):
            info = connection.meta
        elif isinstance(connection, ServerInfo):
            info = connection
        else:
            raise ValueError('unknown type %s' % type(connection))

        if e is None:
            e = sys.exc_info()[1]

        info.penalty = monotonic() + self.penalty
        self._logger.error('Connection to server %s closed: %s.', connection, e)


class ConnectionProviderAsync(_ConnectionProviderBase):
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
                    return self.make_connection((yield from websql.connect(nonblocking=True, loop=self._loop, **info.kwargs)), info)
                except websql.Error as e:
                    self.invalidate(info, e)

        raise RuntimeError('There is no online servers.')


class ConnectionProviderSync(_ConnectionProviderBase):
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
                    return self.make_connection(websql.connect(nonblocking=False, **info.kwargs), info)
                except websql.Error as e:
                    self.invalidate(info, e)

        raise RuntimeError('There is no online servers.')
