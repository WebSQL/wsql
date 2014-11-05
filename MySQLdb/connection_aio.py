__author__ = "Bulat Gaifullin"

from .connections import Connection as _Connection
from .converters import get_codec
from .cursors import Cursor as __Cursor
from .constants import ASYNC
import asyncio


class Result:
    def __init__(self, cursor):
        self.cursor = cursor
        connection = cursor.connection
        self._result = connection.get_result(cursor.use_result)

    def fetchone(self):
        connection = self.cursor.connection
        return connection.future()


class Cursor(__Cursor):
        @asyncio.coroutine
        def _query(self, query):
            """Low-level; executes query, gets result, sets up decoders."""
            connection = self.connection
            self.flush()
            self._executed = query
            yield from connection.query(query)
            self._result = Result(self)

        @asyncio.coroutine
        def execute(self, query, args=None):
            """
                Execute a query.
                :param query -- string, query to execute on server
                :param args -- optional sequence or mapping, parameters to use with query.

                Note: If args is a sequence, then %s must be used as the
                parameter placeholder in the query. If a mapping is used,
                %(key)s must be used as the placeholder.
            """

            connection = self.connection
            self._clear()
            charset = connection.character_set_name
            if isinstance(query, str):
                query = query.encode(charset)
            try:
                if args is not None:
                    query = query % tuple((get_codec(connection, a, self.encoders)(connection, a) for a in args))
                yield from self._query(query)
            except TypeError as e:
                self.errorhandler(connection.ProgrammingError(str(e)))
            except Exception as e:
                self.errorhandler(e)

            if not self._defer_warnings:
                self.warning_check()





class Connection(_Connection):
    _Future = asyncio.Future
    NET_ASYNC_READ = ASYNC.NET_ASYNC_READ
    NET_ASYNC_WRITE = ASYNC.NET_ASYNC_WRITE
    NET_ASYNC_CONNECT = ASYNC.NET_ASYNC_CONNECT
    NET_STATUS_COMPLETE = ASYNC.NET_STATUS_COMPLETE
    NET_STATUS_NOT_READY = ASYNC.NET_STATUS_NOT_READY

    def __init__(self, *args, **kwargs):
        kwargs['nonblocking'] = True
        self._loop = kwargs.pop('loop', asyncio.get_event_loop())
        super().__init__(*args, **kwargs)

    def add_read_callback(self, callback, *args):
        self._loop.add_reader(self._db.fd, callback, *args)

    def add_write_callback(self, callback, *args):
        self._loop.add_writer(self._db.fd, callback, *args)

    def remove_read_callback(self):
        self._loop.remove_reader(self._db.fd)

    def remove_write_callback(self):
        self._loop.remove_writer(self._db.fd)

    def remove_callback(self):
        operation = self._db.async_operation
        if operation == self.NET_ASYNC_READ:
            self.remove_read_callback()
        elif operation == self.NET_ASYNC_WRITE or operation == self.NET_ASYNC_CONNECT:
            self.remove_write_callback()
        else:
            raise RuntimeError("Unexpected async operation: %d" % operation)

    def add_callback(self, callback, *args):
        operation = self._db.async_operation
        if operation == self.NET_ASYNC_READ:
            self.add_read_callback(callback, *args)
        elif operation == self.NET_ASYNC_WRITE or operation == self.NET_ASYNC_CONNECT:
            self.add_write_callback(callback, *args)
        else:
            raise RuntimeError("Unexpected async operation: %d" % operation)

    def do_async(self, fut, registered, func, *args):
        if registered:
            self.remove_callback()
        if fut.cancelled():
            return
        try:
            status = func(*args)
            if status[0] == self.NET_STATUS_COMPLETE:
                fut.set_result(status[1])
            elif status[0] == self.NET_STATUS_NOT_READY:
                self.add_callback(self.do_async, fut, True, func, *args)
            else:
                raise RuntimeError("Unexpected async status: %d" % status)
        except Exception as exc:
            fut.set_exception(exc)

    def future(self, callback, *args):
        fut = self._Future(loop=self._loop)
        self.do_async(fut, False, callback, *args)
        return fut

    def start(self):
        return self.future(self._db.run_async)

    def query(self, query):
        return self.future(self._db.query_async, query)

    @asyncio.coroutine
    def show_warnings(self):
        """Return detailed information about warnings as a sequence of tuples
        of (Level, Code, Message). This is only supported in MySQL-4.1 and up.
        If your server is an earlier version, an empty sequence is returned.

        Non-standard. This is invoked automatically after executing a query,
        so you should not usually call it yourself."""
        if self._server_version < (4, 1):
            return tuple()

        with self.cursor(self) as cursor:
            yield from cursor.execute("SHOW WARNINGS")
            yield from cursor.fetchall()


@asyncio.coroutine
def connect(*args, sql_mode=None, charset=None, **kwargs):
    """Factory function for asynchronous connections.Connection."""

    connection = Connection(*args, **kwargs)
    yield from connection.start()
    connection.setup(sql_mode, charset)
    return connection
