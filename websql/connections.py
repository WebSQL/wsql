"""
WebSQL Connections
------------------

This module implements connections for WebSQL.
You should not try to
create Connection directly; use connect method instead.
"""

__author = "@bg"
from .converters import default_decoders, default_encoders, default_row_formatter
import _websql
import asyncio
import weakref

UNSET = object()


__all__ = ['connect', 'Warning', 'Error', 'InterfaceError', 'DatabaseError', 'DataError', 'OperationalError',
           'IntegrityError', 'InternalError', 'ProgrammingError', 'NotSupportedError', 'StandardError']

Warning = _websql.exceptions.Warning
Error = _websql.exceptions.Error
InterfaceError = _websql.exceptions.InterfaceError
DatabaseError = _websql.exceptions.DatabaseError
DataError = _websql.exceptions.DataError
OperationalError = _websql.exceptions.OperationalError
IntegrityError = _websql.exceptions.IntegrityError
InternalError = _websql.exceptions.InternalError
ProgrammingError = _websql.exceptions.ProgrammingError
NotSupportedError = _websql.exceptions.NotSupportedError
StandardError = _websql.exceptions.StandardError


def connect(*args, loop=UNSET, **kwargs):
    """
    Create a connection to the database. It is strongly recommended that
    you only use keyword parameters. Consult the MySQL C API documentation
    for more information.

    :param host: the hostname
    :type host: str
    :param user: user to connect as
    :type user: str
    :param password: password to use
    :type password: str
    :param database: database to use
    :type database: str
    :param port: TCP/IP port to connect to
    :type port: int
    :param socket_name: the UNIX socket name to use for connections to "localhost" under UNIX
                        or the pipe name for named pipe connections under Windows.
    :type socket_name: str
    :param decoders: SQL decoder stack
    :type decoders: list|tuple
    :param encoders: SQL encoder stack
    :type encoders: list|tuple
    :param row_formatter: the function to format row
    :param connect_timeout: number of seconds to wait before the connection attempt fails.
    :type connect_timeout: number
    :param compress: if set, compression is enabled
    :type compress: bool
    :param init_command: command which is run once the connection is created
    :type init_command: str
    :param read_default_file: file from which default client values are read
    :type read_default_file: str
    :param read_default_group: configuration group to use from the default file
    :type read_default_group: str
    :param use_unicode: If True, text-like columns are returned as unicode objects
                        using the connection's character set.  Otherwise, text-like
                        columns are returned as strings.  columns are returned as
                        normal strings. Unicode objects will always be encoded to
                        the connection's character set regardless of this setting.
    :type use_unicode: bool
    :param charset:     If supplied, the connection character set will be changed
                        to this character set (MySQL-4.1 and newer). This implies use_unicode=True.
    :type charset: str
    :param sql_mode:    If supplied, the session SQL mode will be changed to this
                        setting (MySQL-4.1 and newer). For more details and legal
                        values, see the MySQL documentation.
    :type sql_mode: str
    :param client_flag: flags to use or 0 (see MySQL docs or constants.CLIENT_*)
    :type sql_mode: int
    :param ssl:         dictionary or mapping, contains SSL connection parameters;
                        see the MySQL documentation for more details
                        (mysql_ssl_set()).  If this is set, and the client does not
                        support SSL, NotSupportedError will be raised.
    :type ssl: dict|mapping
    :param local_infile: integer, non-zero enables LOAD LOCAL INFILE; zero disables
    :type local_infile: bool
    :param nonblocking: if set, will be used asynchronous API,
                        If this is set, and client does not support non-blocking, NotSupportedError will be raised.
    :type nonblocking: bool
    :return: new coroutine in nonblocking mode and connection otherwise
    """

    if loop is not UNSET:
        return connection_promise(loop=loop, **kwargs)
    return Connection(*args, **kwargs)


@asyncio.coroutine
def connection_promise(*args, sql_mode=None, charset=None, **kwargs):
    """
    Factory function for asynchronous connections.Connection.
    for details see connect
    """

    connection = AsyncConnection(*args, **kwargs)
    yield from connection.start()
    connection.setup(charset, sql_mode)
    return connection


class ConnectionBase(object):
    """
    Base class for connection implementations
    """
    Error = _websql.exceptions.Error
    InterfaceError = _websql.exceptions.InterfaceError
    DatabaseError = _websql.exceptions.DatabaseError
    DataError = _websql.exceptions.DataError
    OperationalError = _websql.exceptions.OperationalError
    IntegrityError = _websql.exceptions.IntegrityError
    InternalError = _websql.exceptions.InternalError
    ProgrammingError = _websql.exceptions.ProgrammingError
    NotSupportedError = _websql.exceptions.NotSupportedError
    Warning = _websql.exceptions.Warning

    _db = None

    def __init__(self, cursorclass, *args,
                 encoders=None,
                 decoders=None,
                 row_formatter=None,
                 client_flag=None, **kwargs):
        """
        :param cursorclass: the cursor class
        :param args: connection args, for details see connect
        :param encoders: the list of functions to convert from python to sql
        :param decoders: the list of functions to convert from sql to python
        :param row_formatter: the function to format row
        :param client_flag: the flags of client, for details see connect
        :param kwargs: connection keyword arguments, for details see connect
        """

        self.cursorclass = cursorclass
        self.encoders = default_encoders if encoders is None else encoders
        self.decoders = default_decoders if decoders is None else decoders
        self.row_formatter = default_row_formatter if row_formatter is None else row_formatter
        self.format = _websql.format

        client_flag = client_flag or 0
        client_version = tuple((int(n) for n in _websql.get_client_info().split('.')[:2]))
        if client_version >= (4, 1):
            client_flag |= _websql.constants.CLIENT_MULTI_STATEMENTS
        if client_version >= (5, 0):
            client_flag |= _websql.constants.CLIENT_MULTI_RESULTS

        self._db = _websql.connect(*args, client_flag=client_flag, **kwargs)
        self.messages = []
        self._server_version = None
        weakref.finalize(self, lambda db: db.closed or db.close(), db=self._db)

    def __getattr__(self, item):
        """get attribute"""
        return getattr(self._db, item)

    def cursor(self):
        encoders = self.encoders
        """
            Create a cursor instance of cursorclass on which queries may be performed.
            :return the new cursor object
            :rtype cursorclass
        """
        decoders = self.decoders
        row_formatter = self.row_formatter
        return self.cursorclass(self, encoders, decoders, row_formatter)


class Connection(ConnectionBase):
    """The Synchronous Connection Implementation"""

    def __init__(self, *args, charset=None, sql_mode=None, **kwargs):
        """
        :param args: connection args, for details see connect
        :param charset: the connection charset
        :param sql_mode: the connection mode
        :param kwargs: connection keyword arguments, for details see connect
        """

        from .cursors import Cursor
        super().__init__(Cursor, *args, **kwargs)

        if charset:
            self._db.charset = charset
        if sql_mode:
            self._db.set_sql_mode(sql_mode)

        self._db.autocommit = False
        self._server_version = tuple(int(n) for n in self._db.server_info.split('.')[:2])

    def __enter__(self):
        """context object method"""
        return self.cursor()

    def __exit__(self, exc, *_):
        """context object method"""
        if exc:
            self.rollback()
        else:
            self.commit()

    def query(self, query):
        """
        Note: do not use this method directly, use cursor instead
        :param query: the sql query to perform
        :type query: bytes|str
        :return: None
        """
        if isinstance(query, str):
            query = query.encode(self._db.charset)
        return self._db.query(query)

    def show_warnings(self):
        """
        Non-standard. This is invoked automatically after executing a query,
        so you should not usually call it yourself.

        :return  detailed information about warnings as a sequence of tuples
        of (Level, Code, Message). This is only supported in MySQL-4.1 and up.
        If your server is an earlier version, it will be an empty sequence.
        :rtype tuple
        """
        if self._server_version < (4, 1):
            return tuple()
        self._db.query(b"SHOW WARNINGS")
        return tuple(self._db.get_result(False))

    def ping(self, reconnect=False):
        """
        check that connection to server is still alive
        if connection lost and cannot reconnect, the exception will be raised
        :param reconnect: if True, in case if connection is lost, the cline will try to reconnect
        :return None
        """

        if reconnect and not self._db.autocommit:
            raise self.ProgrammingError("autocommit must be enabled before enabling auto-reconnect; consider the consequences")
        return self._db.ping(reconnect)


class AsyncConnection(ConnectionBase):
    """The asynchronous connection implementation"""

    _Future = asyncio.Future
    NET_ASYNC_WRITE = _websql.constants.NET_ASYNC_WRITE
    NET_ASYNC_READ = _websql.constants.NET_ASYNC_READ
    NET_ASYNC_CONNECT = _websql.constants.NET_ASYNC_CONNECT
    NET_ASYNC_COMPLETE = _websql.constants.NET_ASYNC_COMPLETE
    NET_ASYNC_NOT_READY = _websql.constants.NET_ASYNC_NOT_READY

    def __init__(self, *args, loop=None, **kwargs):
        """
        :param args: connection args, for details see connect
        :param loop: the event loop, Optional, by default the current loop will be used
        :param kwargs: connection keyword arguments, for details see connect
        """
        from .cursors import AsyncCursor
        super().__init__(AsyncCursor, *args, nonblocking=True, **kwargs)
        self._loop = asyncio.get_event_loop() if loop is None else loop
        self._server_version = None

    def setup(self, charset, sql_mode):
        """
        :param charset:  If supplied, the connection character set will be changed
                         to this character set (MySQL-4.1 and newer). This implies use_unicode=True.
        :type charset: str
        :param sql_mode: If supplied, the session SQL mode will be changed to this
                         setting (MySQL-4.1 and newer). For more details and legal
                         values, see the MySQL documentation.
        :type sql_mode: str
        :return: None
        """
        self._server_version = tuple(int(n) for n in self._db.server_info.split('.')[:2])
        if charset:
            self._db.charset = charset
        if sql_mode:
            self._db.set_sql_mode(sql_mode)
        self._db.autocommit = False

    def start(self):
        """
        start asynchronous connection
        Non-standard
        :return promise
        :rtype Future
        """
        return self.promise(self._db.run_async)

    def query(self, query):
        """
        :param query: string query
        :type query: str|bytes
        :return promise
        :rtype Future
        """
        return self.promise(self._db.query_async, query)

    def next_result(self):
        """
        next result set (internal)
        :return promise
        :rtype Future
        """
        return self.promise(self._db.next_result_async)

    def commit(self):
        """
        commit transaction
        :return promise
        :rtype Future
        """
        return self.query(b"COMMIT")

    def rollback(self):
        """
        rollback transaction
        :return promise
        :rtype Future
        """
        return self.query(b"ROLLBACK")

    @asyncio.coroutine
    def show_warnings(self):
        """
        Non-standard. This is invoked automatically after executing a query,
        so you should not usually call it yourself.

        :return list, the detailed information about warnings as a sequence of tuples
        of (Level, Code, Message). This is only supported in MySQL-4.1 and up.
        If your server is an earlier version, an empty sequence is returned.
        :rtype list
        """
        if self._server_version < (4, 1):
            return list()

        yield from self.query(b"SHOW WARNINGS")
        result = self.get_result(False)
        warnings = list()
        while True:
            row = yield from self.promise(result.fetch_row_async)
            if row is None:
                break
            warnings.append(row)
        return warnings

    def add_read_callback(self, callback, *args):
        """
        register the read callback on self socket
        :param callback: callable object, that will be called on event
        :param args: arguments that will be passed to callback
        :return: None
        """
        self._loop.add_reader(self._db.fd, callback, *args)

    def add_write_callback(self, callback, *args):
        """
        register the write callback on self socket
        :param callback: callable object, that will be called on event
        :param args: arguments that will be passed to callback
        :return: None
        """
        self._loop.add_writer(self._db.fd, callback, *args)

    def remove_read_callback(self):
        """
        remove read callback
        :return: None
        """
        self._loop.remove_reader(self._db.fd)

    def remove_write_callback(self):
        """
        remove read callback
        :return: None
        """
        self._loop.remove_writer(self._db.fd)

    def remove_callback(self):
        """
        remove previously registered callback
        :return: None
        """
        operation = self._db.async_operation
        if operation == self.NET_ASYNC_READ:
            self.remove_read_callback()
        elif operation == self.NET_ASYNC_WRITE or operation == self.NET_ASYNC_CONNECT:
            self.remove_write_callback()
        else:
            raise RuntimeError("Unexpected async operation: %d" % operation)

    def add_callback(self, callback, *args):
        """
        register a new callback on socket
        :param: callback: callable object
        :param args: arguments that will be passed to callback
        :return: None
        """

        operation = self._db.async_operation
        if operation == self.NET_ASYNC_READ:
            self.add_read_callback(callback, *args)
        elif operation == self.NET_ASYNC_WRITE or operation == self.NET_ASYNC_CONNECT:
            self.add_write_callback(callback, *args)
        else:
            raise RuntimeError("Unexpected async operation: %d" % operation)

    def wrap_callback(self, fut, registered, func, *args):
        """
        Wrap the callback
        :param fut: Future
        :param registered: flag, True if callback was register before
        :param func: callback method
        :param args: arguments that will be passed to callback
        :return: None
        """
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
                raise RuntimeError("Unexpected async status: %d" % status[0])
        except Exception as exc:
            fut.set_exception(exc)

    def promise(self, func, *args):
        """
        Wrap non-blocking call and return promise
        :param func: non-blocking function
        :param args: function arguments
        :return: promis
        :rtype Future
        """
        fut = self._Future(loop=self._loop)
        self.wrap_callback(fut, False, func, *args)
        return fut
