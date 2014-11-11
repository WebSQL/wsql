"""
MySQLdb Connections
-------------------

This module implements connections for MySQLdb. Presently there is
only one class: Connection. Others are unlikely. However, you might
want to make your own subclasses. In most cases, you will probably
override Connection.default_cursor with a non-standard Cursor class.

"""

from . import _mysql
from ._mysql import constants
from ._mysql import exceptions

import weakref
import asyncio


__all__ = ['connect', 'Warning', 'Error', 'InterfaceError', 'DatabaseError', 'DataError', 'OperationalError',
           'IntegrityError', 'InternalError', 'ProgrammingError', 'NotSupportedError']

Warning = exceptions.Warning
Error = exceptions.Error
InterfaceError = exceptions.InterfaceError
DatabaseError = exceptions.DatabaseError
DataError = exceptions.DataError
OperationalError = exceptions.OperationalError
IntegrityError = exceptions.IntegrityError
InternalError = exceptions.InternalError
ProgrammingError = exceptions.ProgrammingError
NotSupportedError = exceptions.NotSupportedError


def connect(*args, nonblocking=None, **kwargs):
    """
    Create a connection to the database. It is strongly recommended that
    you only use keyword parameters. Consult the MySQL C API documentation
    for more information.

    :param host: host to connect
    :param user: user to connect as
    :param password: password to use
    :param database: database to use
    :param port: TCP/IP port to connect to
    :param socket_name: the UNIX socket name to use for connections to "localhost" under UNIX
                        or the pipe name for named pipe connections under Windows.
    :param decoders: SQL decoder stack
    :param encoders: SQL encoder stack
    :param connect_timeout: number of seconds to wait before the connection attempt fails.
    :param compress: if set, compression is enabled
    :param init_command: command which is run once the connection is created
    :param read_default_file: file from which default client values are read
    :param read_default_group: configuration group to use from the default file
    :param use_unicode: If True, text-like columns are returned as unicode objects
                        using the connection's character set.  Otherwise, text-like
                        columns are returned as strings.  columns are returned as
                        normal strings. Unicode objects will always be encoded to
                        the connection's character set regardless of this setting.

    :param charset:     If supplied, the connection character set will be changed
                        to this character set (MySQL-4.1 and newer). This implies use_unicode=True.

    :param sql_mode:    If supplied, the session SQL mode will be changed to this
                        setting (MySQL-4.1 and newer). For more details and legal
                        values, see the MySQL documentation.

    :param client_flag: integer, flags to use or 0 (see MySQL docs or constants.CLIENT_*)

    :param ssl:         dictionary or mapping, contains SSL connection parameters;
                        see the MySQL documentation for more details
                        (mysql_ssl_set()).  If this is set, and the client does not
                        support SSL, NotSupportedError will be raised.

    :param local_infile: integer, non-zero enables LOAD LOCAL INFILE; zero disables

    :param nonblocking: if set, will be used asynchronous API,
                        If this is set, and client does not support non-blocking, NotSupportedError will be raised.

    :return: new coroutine in nonblocking mode and connection otherwise
    """

    if nonblocking:
        return connection_promise(**kwargs)
    return Connection(*args, **kwargs)


@asyncio.coroutine
def connection_promise(*args, sql_mode=None, charset=None, **kwargs):
    """Factory function for asynchronous connections.Connection."""

    connection = ConnectionAsync(*args, **kwargs)
    yield from connection.start()
    connection.setup(sql_mode, charset)
    return connection


def default_error_handler(obj, error):
    """
    If cursor is not None, (errorclass, errorvalue) is appended to
    cursor.messages; otherwise it is appended to connection.messages. Then
    errorclass is raised with errorvalue as the value.

    You can override this with your own error handler by assigning it to the
    instance.
    """
    if obj:
        obj.messages.append(error)
    obj.close()
    raise error


class ConnectionBase(object):
    Error = Error
    InterfaceError = InterfaceError
    DatabaseError = DatabaseError
    DataError = DataError
    OperationalError = OperationalError
    IntegrityError = IntegrityError
    InternalError = InternalError
    ProgrammingError = ProgrammingError
    NotSupportedError = NotSupportedError

    errorhandler = default_error_handler
    _db = None

    def __init__(self, cursorclass, *args,
                 encoders=None,
                 decoders=None,
                 row_formatter=None,
                 client_flag=None, **kwargs):

        from ._mysql import constants
        from .converters import default_decoders, default_encoders, default_row_formatter

        self.cursorclass = cursorclass
        self.encoders = default_encoders if encoders is None else encoders
        self.decoders = default_decoders if decoders is None else decoders
        self.row_formatter = default_row_formatter if row_formatter is None else row_formatter

        client_flag = client_flag or 0
        client_version = tuple((int(n) for n in _mysql.get_client_info().split('.')[:2]))
        if client_version >= (4, 1):
            client_flag |= constants.CLIENT_MULTI_STATEMENTS
        if client_version >= (5, 0):
            client_flag |= constants.CLIENT_MULTI_RESULTS

        self._db = _mysql.connect(*args, client_flag=client_flag, **kwargs)
        self.messages = []
        self._active_cursor = None
        self._server_version = None
        weakref.finalize(self, lambda db: not db.closed and db.close(), db=self._db)

    def __getattr__(self, item):
        return getattr(self._db, item)

    def cursor(self):
        """
            Create a cursor on which queries may be performed.
            The optional
            cursorclass parameter is used to create the Cursor. By default,
            self.cursorclass=cursors.Cursor is used.
        """
        active_cursor = self._active_cursor and self._active_cursor()
        if active_cursor:
            self._active_cursor.close()

        encoders = self.encoders
        decoders = self.decoders
        row_formatter = self.row_formatter
        active_cursor = self.cursorclass(self, encoders, decoders, row_formatter)
        self._active_cursor = weakref.ref(active_cursor)
        return active_cursor


class Connection(ConnectionBase):
    """MySQL Database Connection Object"""

    def __init__(self, *args, charset=None, sql_mode=None, **kwargs):
        from .cursors import Cursor
        super().__init__(Cursor, *args, **kwargs)

        if charset:
            self._db.character_set_name = charset
        if sql_mode:
            self._db.set_sql_mode(sql_mode)

        self._server_version = tuple(int(n) for n in self._db.server_info.split('.')[:2])

    def __enter__(self):
        return self.cursor()

    def __exit__(self, exc, *_):
        if exc:
            self.rollback()
        else:
            self.commit()

    def show_warnings(self):
        """
        Return detailed information about warnings as a sequence of tuples
        of (Level, Code, Message). This is only supported in MySQL-4.1 and up.
        If your server is an earlier version, an empty sequence is returned.

        Non-standard. This is invoked automatically after executing a query,
        so you should not usually call it yourself.
        """
        if self._server_version < (4, 1):
            return tuple()
        self._db.query(b"SHOW WARNINGS")
        return tuple(self._db.get_result(False))

    def ping(self, reconnect=False):
        if reconnect and not self._db.autocommit:
            raise self.ProgrammingError("autocommit must be enabled before enabling auto-reconnect; consider the consequences")
        return self._db.ping(reconnect)


class ConnectionAsync(ConnectionBase):
    _Future = asyncio.Future
    NET_ASYNC_WRITE = constants.NET_ASYNC_WRITE
    NET_ASYNC_READ = constants.NET_ASYNC_READ
    NET_ASYNC_CONNECT = constants.NET_ASYNC_CONNECT
    NET_ASYNC_COMPLETE = constants.NET_ASYNC_COMPLETE
    NET_ASYNC_NOT_READY = constants.NET_ASYNC_NOT_READY

    def __init__(self, *args, loop=None, **kwargs):
        from .cursors import CursorAsync
        super().__init__(CursorAsync, *args, nonblocking=True, **kwargs)
        self._loop = asyncio.get_event_loop() if loop is None else loop
        self._server_version = None

    def setup(self, charset, sql_mode):
        self._server_version = tuple(int(n) for n in self._db.server_info.split('.')[:2])
        if charset:
            self._db.character_set_name = charset
        if sql_mode:
            self._db.set_sql_mode(sql_mode)

    def start(self):
        """
        start asynchronous connection
        Non-standard
        :return promise
        """
        return self.promise(self._db.run_async)

    def query(self, query):
        """
        :param query: string query
        :return: promise
        """
        return self.promise(self._db.query_async, query)

    def next_result(self):
        """
        next result set (internal)
        :return: promise
        """
        return self.promise(self._db.next_result_async)

    def commit(self):
        """
        commit transaction
        :return: promise
        """
        return self.query(b"COMMIT")

    def rollback(self):
        """
        rollback transaction
        :return: promise
        """
        return self.query(b"ROLLBACK")

    @asyncio.coroutine
    def show_warnings(self):
        """
        :return list, the detailed information about warnings as a sequence of tuples
        of (Level, Code, Message). This is only supported in MySQL-4.1 and up.
        If your server is an earlier version, an empty sequence is returned.

        Non-standard. This is invoked automatically after executing a query,
        so you should not usually call it yourself.
        """
        if self._server_version < (4, 1):
            return list()

        yield from self.query(b"SHOW WARNINGS")
        result = self.get_result(False)
        warnings = list()
        while True:
            row = yield from self.promise(result.fetch_row)
            if row is None:
                break
            warnings.append(row)
        return warnings

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

    def wrap_callback(self, fut, registered, func, *args):
        if registered:
            self.remove_callback()
        if fut.cancelled():
            return
        try:
            status = func(*args)
            if status[0] == self.NET_ASYNC_COMPLETE:
                fut.set_result(status[1])
            elif status[0] == self.NET_ASYNC_NOT_READY:
                self.add_callback(self.wrap_callback, fut, True, func, *args)
            else:
                raise RuntimeError("Unexpected async status: %d" % status)
        except Exception as exc:
            fut.set_exception(exc)

    def now(self, value):
        fut = self._Future()
        fut.set_result(value)
        return fut

    def promise(self, callback, *args):
        fut = self._Future(loop=self._loop)
        self.wrap_callback(fut, False, callback, *args)
        return fut
