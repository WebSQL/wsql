"""
MySQLdb Cursors
---------------

This module implements the Cursor class. You should not try to
create Cursors direction; use connection.cursor() instead.

"""

from ._websql import constants
from ._websql import exceptions
from .converters import get_codec
from warnings import warn
import asyncio
import re
import weakref


INSERT_VALUES = re.compile(br"(?P<start>.+values\s*)"
                           br"(?P<values>\(((?<!\\)'[^\)]*?\)[^\)]*(?<!\\)?'|[^\(\)]|(?:\([^\)]*\)))+\))"
                           br"(?P<end>.*)", re.I)


class CursorBase(object):
    """
    A base for Cursor classes.
    """

    use_result = False
    _defer_warnings = False

    ProgrammingError = exceptions.ProgrammingError
    StandardError = exceptions.StandardError
    NotSupportedError = exceptions.NotSupportedError

    def __init__(self, connection, encoders, decoders, row_formatter):
        self.messages = []
        self.encoders = encoders
        self.decoders = decoders
        self.row_formatter = row_formatter
        self.errorhandler = connection.errorhandler
        self.arraysize = 1
        self._rowcount = -1
        self._connection = weakref.ref(connection)
        self._executed = None
        self._result = None
        self._pending_results = []
        self._warnings = 0
        self._info = None
        self._row_decoders = ()

    def _check_executed(self):
        """Ensure that .execute() has been called."""
        if not self._executed:
            self.errorhandler(self.ProgrammingError("execute() first"))

    def _push(self):
        """store current result"""
        if self._result is not None:
            result = self._result
            self._result = None
            self._pending_results.append(result)

    def _pop(self):
        if self._pending_results:
            self._result = self._pending_results[0]
            del self._pending_results[0]
            return True
        return False

    def _acquire_result(self, connection=None):
        connection = connection or self._connection()
        result = connection.get_result(self.use_result)
        if result is not None:
            decoders = self.decoders
            self._row_decoders = tuple(get_codec(connection, field, decoders) for field in result.fields)
            self._rowcount = result.num_rows
            self._result = result

    def _release_result(self):
        if self._result is not None:
            result = self._result
            self._result = None
            self._rowcount = -1
            self._row_decoders = ()
            return result

    def _encode(self, connection, obj):
        """
        Given an object obj, returns an SQL literal as a string. Non-standard.
        :param obj: object to encode
        :return sql literal
        """
        return get_codec(connection, obj, self.encoders[:-1])(connection, obj)

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
        :return:  the current 0-based index of the cursor in the result set or None if the index cannot be determined.
        """
        if self._result:
            return self._result.tell()

    @property
    def lastrowid(self):
        """
        :return: he rowid of the last modified row
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
    use_result = False

    def _release_result(self):
        result = super()._release_result()
        if result is not None:
            try:
                while result.fetch_row() is not None:
                    pass
                result.free()
            except self.StandardError:
                pass

    def close(self):
        while self.nextset():
            pass
        self._result = None

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
        self._release_result()
        if self._pop():
            return True

        connection = self.connection
        if connection.next_result():
            self._acquire_result()
            return True
        return False

    def execute(self, query, args=None):
        """
        Execute a query.
        :param query: string, query to execute on server
        :param args: optional sequence or mapping, parameters to use with query.
                     Note: If args is a sequence, then %s must be used as the
                     parameter placeholder in the query. If a mapping is used,
                     %(key)s must be used as the placeholder.
        """
        connection = self.connection
        try:
            if isinstance(query, str):
                query = query.encode(connection.charset)

            if args is not None:
                query = connection.format(query, tuple(get_codec(connection, a, self.encoders)(connection, a) for a in args))
            self._query(query)

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
        connection = self.connection
        if not args:
            return

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
            self._query(multi_row_query)

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

        connection = self.connection
        try:
            if isinstance(procname, str):
                procname = procname.encode(connection.charset)

            for index, arg in enumerate(args):
                query = connection.format(b"SET @_%s_%d=%s", (procname, index, self._encode(connection, arg)))
                self._query(query)
                self.nextset()

            query = connection.format(b"CALL %s(%s)",
                                            (procname,
                                             b','.join(connection.format(
                                                 b'@_%s_%d',
                                                 (procname, i)) for i in range(len(args)))))

            self._query(query)

            if not self._defer_warnings:
                self.warning_check()
            return args
        except TypeError as e:
            self.errorhandler(self.ProgrammingError(e))
        except Exception as e:
            self.errorhandler(e)

    def __iter__(self):
        return iter(self.fetchone, None)

    def _query(self, query):
        """Low-level; executes query, gets result, sets up decoders."""
        connection = self.connection
        self._push()
        self._executed = query
        connection.query(query)
        self._acquire_result()

    def fetchone(self):
        """
        Fetches a single row from the cursor. None indicates that
        no more rows are available.
        :return formatted row or None if there is no more rows
        """
        self._check_executed()
        if self._result is None:
            return None
        return self.row_formatter(self._row_decoders, self._result.fetch_row())

    def fetchmany(self, size=None):
        """Fetch up to size rows from the cursor. Result set may be smaller
        than size. If size is not defined, cursor.arraysize is used."""
        self._check_executed()
        if self._result is None:
            return []
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
        self._check_executed()
        if self._result is None:
            return []
        return [row for row in self]

    def scroll(self, offset, mode='relative'):
        """
        Scroll the cursor in the result set to a new position according to mode.
        If mode is 'relative' (default), value is taken as offset to
        the current position in the result set, if set to 'absolute',
        value states an absolute target position.
        """
        origin = 0
        self._check_executed()
        if mode == 'relative':
            origin = 1  # SEEK_CUR
        elif mode == 'absolute':
            origin = 0  # SEEK_SET
        else:
            self.errorhandler(self.ProgrammingError("unknown scroll mode %s" % (mode,)))

        self._result.row_seek(offset, origin)


class CursorAsync(CursorBase):
    class _NullResult:
        @staticmethod
        def fetch_row_async():
            return constants.NET_ASYNC_COMPLETE, None

        free_async = fetch_row_async

    use_result = True

    @asyncio.coroutine
    def _release_result(self):
        result = super()._release_result()
        if result is not None:
            result = self._result
            connection = self.connection
            self._result = None
            try:
                while (yield from connection.promise(result.fetch_row_async)) is not None:
                    pass

                yield from connection.promise(result.free_async)
            except self.StandardError:
                pass

    @asyncio.coroutine
    def close(self):
        while (yield from self.nextset()):
            pass
        self._result = None

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
        yield from self._release_result()
        if self._pop():
            return True

        connection = self.connection
        if (yield from connection.next_result()):
            self._acquire_result()
            return True
        return False

    @asyncio.coroutine
    def execute(self, query, args=None):
        """
        Execute a query.
        :param query: string, query to execute on server
        :param args: optional sequence or mapping, parameters to use with query.
                     Note: If args is a sequence, then %s must be used as the
                     parameter placeholder in the query. If a mapping is used,
                     %(key)s must be used as the placeholder.
        """
        connection = self.connection
        try:
            if isinstance(query, str):
                query = query.encode(connection.charset)

            if args is not None:
                query = connection.format(query, tuple(get_codec(connection, a, self.encoders)(connection, a) for a in args))

            yield from self._query(query)

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
        This method improves performance on multiple-row INSERT and
        REPLACE. Otherwise it is equivalent to looping over args with
        execute().

        :param query: string, query to execute on server
        :param args: Sequence of sequences or mappings, parameters to use with query.
        """
        connection = self.connection
        if not args:
            return

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
            yield from self._query(multi_row_query)

            if not self._defer_warnings:
                yield from self.warning_check()
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

        connection = self.connection
        try:
            if isinstance(procname, str):
                procname = procname.encode(connection.charset)

            for index, arg in enumerate(args):
                query = connection.format(b"SET @_%s_%d=%s", (procname, index, self._encode(connection, arg)))
                yield from self._query(query)
                yield from self.nextset()

            query = connection.format(b"CALL %s(%s)", (procname,
                                                             b','.join(connection.format(
                                                                 b'@_%s_%d',
                                                                 ((procname, i) for i in range(len(args)))))))
            yield from self._query(query)
            if not self._defer_warnings:
                yield from self.warning_check()

            return args
        except TypeError as e:
            self.errorhandler(self.ProgrammingError(e))
        except Exception as e:
            self.errorhandler(e)

    @asyncio.coroutine
    def _query(self, query):
        """Low-level; executes query, gets result, sets up decoders."""
        connection = self.connection
        self._push()
        self._executed = query
        yield from connection.query(query)
        self._acquire_result()

    @asyncio.coroutine
    def fetchone(self):
        """
        Fetches a single row from the cursor. None indicates that
        no more rows are available.
        :return formatted row or None if there is no more rows
        """
        self._check_executed()
        if self._result is None:
            return None
        connection = self.connection
        row = yield from connection.promise(self._result.fetch_row_async)
        if row is None:
            return row
        return self.row_formatter(self._row_decoders, row)

    @asyncio.coroutine
    def fetchmany(self, size=None):
        """Fetch up to size rows from the cursor. Result set may be smaller
        than size. If size is not defined, cursor.arraysize is used."""
        self._check_executed()
        if self._result is None:
            return []
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
        rows = []
        self._check_executed()
        if self._result is None:
            return rows

        append = rows.append
        while True:
            row = yield from self.fetchone()
            if row is None:
                break
            append(row)
        return rows
