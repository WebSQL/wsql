"""
WSQL
====
An asynchronous DB API v2.0 compatible interface to MySQL
---------------------------------------------------------
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""
import random
import wsql
import sys

from asyncio import coroutine
from time import monotonic
from wsql.connections import UNSET


__all__ = ["Upstream"]


def upstream(*args, loop=UNSET, **kwargs):
    """
    create a new connection provider
    :param args: connection provider positional arguments
    :param nonblocking: if True, the non-blocking version will be created
    :param kwargs: connection provider keyword arguments
    :return: the new ConnectionProvider
    """
    if loop is not UNSET:
        return _AsyncUpstream(*args, loop=loop, **kwargs)
    return _Upstream(*args, **kwargs)

Upstream = upstream


class ServerInfo:
    def __init__(self, **kwargs):
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


class Connection:
    """database client"""

    def __init__(self, connection, meta=None):
        """
        :param connection: the native connection to database
        :param meta: the meta information associated with this connection
        """
        self._connection = connection
        self._meta = meta

    def __getattr__(self, item):
        """
        compatibility with native connection
        """
        return getattr(self._connection, item)

    @property
    def _loop(self):
        """to support retryable operations"""
        return getattr(self._connection, '_loop')

    def connection(self):
        """
        :return: the native connection
        """
        return self._connection

    @property
    def meta(self):
        """
        get the meta information associated with this connection
        :return: connection meta information
        """
        return self._meta

    def execute(self, query):
        """
        execute the database query
        :param query: - the callable object, that implement logic to query database
        :return the result of query
        """
        return query(self)


class _UpstreamBase:
    """
    Base class for connection providers
    """

    def __init__(self, servers, logger, **kwargs):
        """
        Constructor
        :param servers: the list of servers
        :param logger: the logger object
        :param kwargs: the connection arguments
        """
        def update_kwargs(host, port):
            kw = kwargs.copy()
            if host:
                kw["host"] = host
            if port:
                kw["port"] = port
            return kw

        self._servers = []
        extend = self._servers.extend
        for s in servers:
            extend([ServerInfo(**update_kwargs(s.get('host'), int(s.get('port', 0) or 0)))] * max(int(s.get('count', 1) or 1), 1))

        if self._servers:
            random.shuffle(self._servers)
        else:
            self._servers.append(ServerInfo(**kwargs))

        self._logger = logger
        self.current = 0
        self.count = len(self._servers)
        self.penalty = 60  # 1 min

    def __len__(self):
        return self.count

    def invalidate(self, connection, e=None):
        """
        Invalidate the disconnected connection
        the appropriate server will be banned to ban timeout
        :param connection: the connection object, that contains meta information
        """

        if isinstance(connection, Connection):
            info = connection.meta
        elif isinstance(connection, ServerInfo):
            info = connection
        else:
            raise ValueError('unknown type %s' % type(connection))

        if e is None:
            e = sys.exc_info()[1]

        info.penalty = monotonic() + self.penalty
        self._logger.error('Connection to server %s closed: %s.', connection, e)


class _AsyncUpstream(_UpstreamBase):
    """
    The asynchronous variant of ConnectionProvider
    """

    def __init__(self, servers, logger, loop=None, **kwargs):
        """
        Constructor
        :param servers: the list of ServerInfo objects
        :param logger: the logger object
        :param loop: the event loop
        """
        super().__init__(servers, logger, **kwargs)
        self._loop = loop

    def __next__(self):
        return self._next()

    @coroutine
    def _next(self):
        """
        Create a new connection to database, try one by one all servers.
        if there is no online servers the RuntimeError will be raised
        :return: the connection object
        """
        current = self.current
        time_ = monotonic()
        for idx in ((i + current) % self.count for i in range(self.count)):
            info = self._servers[idx]
            if info.penalty < time_:
                try:
                    return Connection((yield from wsql.connect(loop=self._loop, **info.kwargs)), info)
                except wsql.Error as e:
                    self.invalidate(info, e)

        raise RuntimeError('There is no online servers.')


class _Upstream(_UpstreamBase):
    """
    The synchronous variant of ConnectionProvider
    """

    def __next__(self):
        """
        Create a new connection to database, try one by one all servers.
        if there is no online servers the RuntimeError will be raised
        :return: the connection object
        """

        current = self.current
        time_ = monotonic()
        for idx in ((i + current) % self.count for i in range(self.count)):
            info = self._servers[idx]
            if info.penalty < time_:
                try:
                    return Connection(wsql.connect(**info.kwargs), info)
                except wsql.Error as e:
                    self.invalidate(info, e)

        raise RuntimeError('There is no online servers.')
