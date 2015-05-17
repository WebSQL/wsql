"""
WebSQL Cursors
--------------

This module implements the Cursor classes. You should not try to
create Cursors directly; use connection.cursor() instead.

"""

from .converters import get_codec
from warnings import warn
import _websql
import asyncio
import re
import weakref


INSERT_VALUES = re.compile(br"(?P<start>.+values\s*)"
                           br"(?P<values>\(((?<!\\)'[^\)]*?\)[^\)]*(?<!\\)?'|[^\(\)]|(?:\([^\)]*\)))+\))"
                           br"(?P<end>.*)", re.I)


class CursorBase:
    """
    A base for Cursor classes.
    """

    _use_result = False
    _defer_warnings = False

    ProgrammingError = _websql.exceptions.ProgrammingError
    StandardError = _websql.exceptions.StandardError
    NotSupportedError = _websql.exceptions.NotSupportedError

    def __init__(self, connection, encoders, decoders, row_formatter):
        """
        :param connection: the connection object
        :param encoders: the python object encoders
        :param decoders: the mysql field decoders
        :param row_formatter: the formatter to format mysql-row
        """
        self.messages = []
        self.encoders = encoders
        self.decoders = decoders
        self.row_formatter = row_formatter
        self.arraysize = 1
        self._rowcount = -1
        self._connection = weakref.ref(connection)
        self._result = None
        self._warnings = 0
        self._info = None
        self._row_decoders = ()

        # override from connection
        if connection.use_result is not None:
            self._use_result = connection.use_result
        if connection.defer_warnings is not None:
            self._defer_warnings = connection.defer_warnings

    def __del__(self):
        if self._result is not None:
            warn(ResourceWarning("unclosed %r" % self))

    def errorhandler(self, error):
        """default error handler"""
        self.messages.append((type(error), str(error)))
        raise error

    def _check_has_result(self):
        """Ensure that .execute() has been called."""
        if self._result is None:
            self.errorhandler(self.ProgrammingError("there is no result"))

    def _acquire_result(self, connection):
        """
        read mysql result and construct accosiated objects
        :param connection: mysql connection
        """

        result = connection.get_result(self._use_result)
        if result is not None:
            decoders = self.decoders
            self._row_decoders = tuple(get_codec(connection, field, decoders) for field in result.fields)
            self._rowcount = result.num_rows
            self._result = result
            return True

    def _release_result(self):
        """
        destroy mysql result
        """

        result = self._result
        self._result = None
        self._rowcount = -1
        self._row_decoders = None
        return result

    def _encode(self, connection, obj):
        """
        Given an object obj, returns an SQL literal as a string. Non-standard.
        :param obj: object to encode
        :return sql literal
        """
        return get_codec(connection, obj, self.encoders)(connection, obj)

    @property
    def connection(self):
        """
            Get the database connection.
            Raises ProgrammingError if the connection has been closed.
        """
        connection = self._connection()
        if connection is None:
            self.errorhandler(self.ProgrammingError("connection lost"))
        return connection

    @property
    def description(self):
        """
        This read-only attribute is a sequence of 7-item sequences.
        Each of these sequences contains information describing one result column.
        """
        if self._result:
            return self._result.description
        return None

    @property
    def rowcount(self):
        """
        :return: the number of rows that the last .execute*() produced (for DQL statements like SELECT)
                 or affected (for DML statements like UPDATE or INSERT)
        """
        return self._rowcount

    @property
    def rownumber(self):
        """
        Not supported yet
        :return:  the current 0-based index of the cursor in the result set or None if the index cannot be determined.
        """
        return None

    @property
    def lastrowid(self):
        """
        :return: rowid of the last modified row
                 (most databases return a rowid only when a single INSERT operation is performed).
                 If the operation does not set a rowid or if the database does not support rowids,
                 this attribute should be set to None.
        """
        return self.connection.last_insert_id

    @staticmethod
    def setinputsizes(*args):
        """Does nothing, required by DB API."""
        pass

    @staticmethod
    def setoutputsizes(*args):
        """Does nothing, required by DB API."""
        pass


class Cursor(CursorBase):
    _use_result = False

    def __enter__(self):
        """context manager"""
        return self

    def __exit__(self, *_):
        """context manager"""
        self.close()
        return False

    def _free_result(self):
        """release current result and free all resources"""
        result = self._release_result()
        if result is None:
            return

        connection = self.connection
        try:
            if result.more_rows:
                while result.fetch_row() is not None:
                    pass

            result.free()
        except self.StandardError as e:
            warn(connection.Warning(str(e)))

    def close(self):
        """close cursor and release resources"""
        connection = self._connection()
        if connection:
            while self.nextset():
                pass

    def warning_check(self):
        """Check for warnings, and report via the warnings module."""
        connection = self.connection
        if connection.warning_count:
            warnings = connection.show_warnings()
            if warnings:
                for warning in map(connection.Warning, warnings):
                    self.messages.append(warning)
                    warn(warning, stacklevel=3)
            elif self._info:
                warning = connection.Warning(self._info)
                self.messages.append(warning)
                warn(warning, stacklevel=3)

    def nextset(self):
        """
        Advance to the next result set.
        Returns False if there are no more result sets.
        """
        self._free_result()
        connection = self.connection
        if connection.next_result():
            if self._acquire_result(connection):
                return True

    def execute(self, query, args=None):
        """
        Execute a query.
        :param query: string, query to execute on server
        :param args: optional sequence or mapping, parameters to use with query.
        Note! If args is a sequence, then %s must be used as the
        parameter placeholder in the query. If a mapping is used,
        %(key)s must be used as the placeholder.
        """
        try:
            connection = self.connection
            if isinstance(query, str):
                query = query.encode(connection.charset)

            if args is not None:
                query = connection.format(query, tuple(get_codec(connection, a, self.encoders)(connection, a) for a in args))

            self._query(connection, query)

            if not self._defer_warnings:
                self.warning_check()

        except TypeError as e:
            self.errorhandler(self.ProgrammingError(str(e)))
        except Exception as e:
            self.errorhandler(e)

    def executemany(self, query, args):
        """
        Execute a multi-row query.
        This method improves performance on multiple-row INSERT and
        REPLACE. Otherwise it is equivalent to looping over args with
        execute().

        :param query: string, query to execute on server
        :param args: Sequence of sequences or mappings, parameters to use with query.
        """
        if not args:
            return

        connection = self.connection

        if isinstance(query, str):
            query = query.encode(connection.charset)

        matched = INSERT_VALUES.match(query)
        if not matched:
            rowcount = 0
            for row in args:
                self.execute(query, row)
                rowcount += self._rowcount
            self._rowcount = rowcount
            return

        start = matched.group('start')
        values = matched.group('values')
        end = matched.group('end')

        try:
            sql_params = (connection.format(values, tuple(get_codec(connection, a, self.encoders)(connection, a) for a in row)) for row in args)
            multi_row_query = b'\n'.join([start, b',\n'.join(sql_params), end])
            self._query(connection, multi_row_query)

            if not self._defer_warnings:
                self.warning_check()

        except TypeError as e:
            self.errorhandler(self.ProgrammingError(e))
        except Exception as e:
            self.errorhandler(e)

    def callproc(self, procname, args=()):
        """
        Execute stored procedure procname with args

        :param procname: string, name of procedure to execute on server
        :param args: Sequence of parameters to use with procedure

        :return the original args. (update args for inout parameters additional performance penalty, skip it)
        """

        try:
            connection = self.connection
            if isinstance(procname, str):
                procname = procname.encode(connection.charset)

            if args:
                for statement in (connection.format(b"SET @_%s_%d=%s", (procname, index, self._encode(connection, arg))) for index, arg in enumerate(args)):
                    self._query(connection, statement)
                self.nextset()

            query = connection.format(b"CALL %s(%s)",
                                      (procname,
                                       b','.join(connection.format(b'@_%s_%d', (procname, i)) for i in range(len(args)))))

            self._query(connection, query)

            if not self._defer_warnings:
                self.warning_check()
            return args
        except TypeError as e:
            self.errorhandler(self.ProgrammingError(e))
        except Exception as e:
            self.errorhandler(e)

    def __iter__(self):
        """Iterable object protocol"""
        return iter(self.fetchone, None)

    def _query(self, connection, query):
        """Low-level; executes query, gets result, sets up decoders."""
        self._free_result()
        connection.query(query)
        self._acquire_result(connection)

    def fetchone(self):
        """
        Fetches a single row from the cursor. None indicates that
        no more rows are available.
        :return formatted row or None if there is no more rows
        """
        self._check_has_result()
        return self.row_formatter(self._row_decoders, (x[0] for x in self._result.description), self._result.fetch_row())

    def fetchmany(self, size=None):
        """Fetch up to size rows from the cursor. Result set may be smaller
        than size. If size is not defined, cursor.arraysize is used."""
        self._check_has_result()

        if size is None:
            size = self.arraysize

        rows = []
        append = rows.append
        for _ in range(size):
            row = self.fetchone()
            if row is None:
                break
            append(row)
        return rows

    def fetchall(self):
        """Fetches all available rows from the cursor."""
        self._check_has_result()
        return [row for row in self]

    def fetchxall(self):
        """
        Same as self.fetchall(), self.next()
        Not standard
        """
        r = self.fetchall()
        self.nextset()
        return r

    def scroll(self, offset, mode='relative'):
        """
        Scroll the cursor in the result set to a new position according to mode.
        If mode is 'relative' (default), value is taken as offset to
        the current position in the result set, if set to 'absolute',
        value states an absolute target position.
        Not supported yet
        """
        raise self.NotSupportedError


class AsyncCursor(CursorBase):
    """Asynchronous cursor implementation"""

    NET_ASYNC_COMPLETE = _websql.constants.NET_ASYNC_COMPLETE
    _use_result = True

    class _NullResult:
        @staticmethod
        def fetch_row_async():
            return AsyncCursor.NET_ASYNC_COMPLETE, None

        free_async = fetch_row_async

    @property
    def rowcount(self):
        """number of row in current result, always return -1"""
        # there is no way to get number of rows from async result
        return -1

    @asyncio.coroutine
    def _free_result(self):
        """free all resources associated with current result"""
        result = super()._release_result()
        if result is not None:
            connection = self.connection
            try:
                if result.more_rows:
                    while (yield from connection.promise(result.fetch_row_async)) is not None:
                        pass

                yield from connection.promise(result.free_async)
            except self.StandardError as e:
                warn(connection.Warning(str(e)))

    @asyncio.coroutine
    def close(self):
        """close cursor and free resources"""
        connection = self._connection()
        if connection:
            while (yield from self.nextset()):
                pass

    @asyncio.coroutine
    def warning_check(self):
        """Check for warnings, and report via the warnings module."""
        connection = self.connection
        if connection.warning_count:
            warnings = yield from connection.show_warnings()
            if warnings:
                for warning in map(connection.Warning, warnings):
                    self.messages.append(warning)
                    warn(warning, stacklevel=3)
            elif self._info:
                warning = connection.Warning(self._info)
                self.messages.append(warning)
                warn(warning, stacklevel=3)

    @asyncio.coroutine
    def nextset(self):
        """
        Advance to the next result set.
        Returns False if there are no more result sets.
        """
        yield from self._free_result()
        connection = self.connection
        if (yield from connection.next_result()):
            if self._acquire_result(connection):
                return True

    @asyncio.coroutine
    def execute(self, query, args=None):
        """
        Execute a query.
        :param query: query to execute on server
        :param args: optional sequence or mapping, parameters to use with query.
        Note! If args is a sequence, then %s must be used as the
        parameter placeholder in the query. If a mapping is used,
        %(key)s must be used as the placeholder.
        """

        try:
            connection = self.connection
            if isinstance(query, str):
                query = query.encode(connection.charset)

            if args is not None:
                query = connection.format(query, tuple(get_codec(connection, a, self.encoders)(connection, a) for a in args))

            yield from self._query(connection, query)

            if not self._defer_warnings:
                yield from self.warning_check()
        except TypeError as e:
            self.errorhandler(self.ProgrammingError(str(e)))
        except Exception as e:
            self.errorhandler(e)

    @asyncio.coroutine
    def executemany(self, query, args):
        """
        Execute a multi-row query.
        This method improves performance on multiple-row INSERT and REPLACE.
        Otherwise it is equivalent to looping over args with execute().

        :param query: query to execute on server
        :param args: Sequence of sequences or mappings, parameters to use with query.
        """

        if not args:
            return

        connection = self.connection

        if isinstance(query, str):
            query = query.encode(connection.charset)

        matched = INSERT_VALUES.match(query)
        if not matched:
            for row in args:
                yield from self.execute(query, row)
            return

        start = matched.group('start')
        values = matched.group('values')
        end = matched.group('end')

        try:
            sql_params = (connection.format(values, tuple(get_codec(connection, a, self.encoders)(connection, a) for a in row)) for row in args)
            multi_row_query = b'\n'.join([start, b',\n'.join(sql_params), end])
            yield from self._query(connection, multi_row_query)

            if not self._defer_warnings:
                yield from self.warning_check()
        except TypeError as e:
            self.errorhandler(self.ProgrammingError(e))
        except Exception as e:
            self.errorhandler(e)

    @asyncio.coroutine
    def callproc(self, procname, args=()):
        """
        Execute stored procedure procname with args

        :param procname: string, name of procedure to execute on server
        :param args: Sequence of parameters to use with procedure

        :return the original args. (update args for inout parameters additional performance penalty, skip it)
        """

        try:
            connection = self.connection

            if isinstance(procname, str):
                procname = procname.encode(connection.charset)

            if args:
                for statement in (connection.format(b"SET @_%s_%d=%s", (procname, index, self._encode(connection, arg))) for index, arg in enumerate(args)):
                    yield from self._query(connection, statement)
                yield from self.nextset()

            query = connection.format(b"CALL %s(%s)",
                                      (procname,
                                       b','.join(connection.format(b'@_%s_%d', (procname, i)) for i in range(len(args)))))

            yield from self._query(connection, query)
            if not self._defer_warnings:
                yield from self.warning_check()
            return args
        except TypeError as e:
            self.errorhandler(self.ProgrammingError(e))
        except Exception as e:
            self.errorhandler(e)

    @asyncio.coroutine
    def _query(self, connection, query):
        """Low-level; executes query, gets result, sets up decoders."""
        yield from self._free_result()
        yield from connection.query(query)
        self._acquire_result(connection)

    @asyncio.coroutine
    def fetchone(self):
        """
        Fetches a single row from the cursor. None indicates that
        no more rows are available.
        :return formatted row or None if there is no more rows
        """
        self._check_has_result()

        connection = self.connection
        return self.row_formatter(self._row_decoders, (x[0] for x in self._result.description), (yield from connection.promise(self._result.fetch_row_async)))

    @asyncio.coroutine
    def fetchmany(self, size=None):
        """Fetch up to size rows from the cursor. Result set may be smaller
        than size. If size is not defined, cursor.arraysize is used."""
        self._check_has_result()

        if size is None:
            size = self.arraysize

        rows = []
        append = rows.append
        for _ in range(size):
            row = yield from self.fetchone()
            if row is None:
                break
            append(row)
        return rows

    @asyncio.coroutine
    def fetchall(self):
        """Fetches all available rows from the cursor."""
        self._check_has_result()

        rows = []
        append = rows.append
        while True:
            row = yield from self.fetchone()
            if row is None:
                break
            append(row)
        return rows

    @asyncio.coroutine
    def fetchxall(self):
        """
        Same as self.nextset(), self.fetchall()
        Not standard
        """
        r = (yield from self.fetchall())
        yield from self.nextset()
        return r

    def scroll(self, offset, mode='relative'):
        """
        Scroll the cursor in the result set to a new position according to mode.
        If mode is 'relative' (default), value is taken as offset to
        the current position in the result set, if set to 'absolute',
        value states an absolute target position.
        Not supported yet
        """
        raise self.NotSupportedError

