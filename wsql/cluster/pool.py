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

from asyncio import coroutine, wait_for
from wsql.connections import UNSET


__all__ = ["ConnectionPool"]


def connection_poll(*args, loop=UNSET, **kwargs):
    """
    create a new connection pool
    :param args: connection pool positional arguments
    :param loop: the event loop, if specified, the asynchronous connection pool will be created
    :param kwargs: connection pool keyword arguments
    :return: the new ConnectionPool
    """

    if loop is not UNSET:
        return _AsyncConnectionPool(*args, loop=loop, **kwargs)
    return _ConnectionPool(*args, **kwargs)

ConnectionPool = connection_poll


class _ConnectionPoolBase:
    """
        Base class for connection pools
    """

    def __init__(self, upstream, queue, timeout=None):
        """
        Constructor
        :param upstream: the cluster upstream
        :type upstream: websql.cluster.Upstream
        :param queue: the queue to store connections
        :type queue: Queue
        :param timeout: the timeout in seconds to acquire new connection
        :param timeout: int
        """
        assert queue.maxsize

        self._upstream = upstream
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
            self._upstream.invalidate(connection)


class _AsyncConnectionPool(_ConnectionPoolBase):
    """
        Asynchronous connection pool
        the connections will be created on demand
    """

    def __init__(self, upstream, loop=None, timeout=None):
        """
        Initialize pool
        :param upstream: the upstream
        :param loop: the event loop, by default current event loop will be used
        """
        from asyncio.queues import Queue, QueueEmpty
        from asyncio import TimeoutError, get_event_loop

        super().__init__(upstream, Queue(len(upstream), loop=loop), timeout=timeout)
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
        return (yield from wait_for(next(self._upstream), timeout=self._timeout, loop=self._loop))

    @coroutine
    def execute(self, query):
        """
        execute the database query on connection pool
        :param query: - the callable object, that implement logic to query database
        :return the response to query
        """
        connection = yield from self._acquire()
        try:
            return (yield from connection.execute(query))
        finally:
            self._release(connection)


class _ConnectionPool(_ConnectionPoolBase):
    """
        Synchronous connection pool
        the connections will be created on demand
    """

    TimeoutError = TimeoutError

    def __init__(self, upstream, timeout=None):
        """
        Initialize pool
        :param upstream: the upstream
        :param timeout: the timeout to create a new connection
        """
        from queue import Queue, Empty
        from threading import Lock

        super().__init__(upstream, Queue(len(upstream)), timeout=timeout)
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
            return next(self._upstream)

        try:
            return self._queue.get(timeout=self._timeout)
        except self.QueueEmpty:
            raise self.TimeoutError from None

    def execute(self, query):
        """
        execute the database query on connection pool
        :param query: - the callable object, that implement logic to query database
        :return the response to query
        """
        connection = self._acquire()
        try:
            return connection.execute(query)
        finally:
            self._release(connection)
