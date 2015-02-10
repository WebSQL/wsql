"""
WebSQL Scope Objects
--------------------

This module implements scope objects for connections.
"""

from asyncio import iscoroutine, iscoroutinefunction, coroutine
import _websql

__all__ = ["transaction", "retryable"]


def retryable(connection, count=5, delay=1):
    """
    Make connection execute retryable
    :param connection: the connection object
    :param count: the maximum number of retries
    :param delay: the delay between retries
    :param loop: optional parameter, event loop of asynchronous method
    :return: the Connection, that has retryable method execute
    """
    count = count or 1
    loop = getattr(connection, "_loop", None)
    if loop is not None:
        return AsyncRetryable(connection, loop=loop, count=count, delay=delay)
    return _Retryable(connection, count=count, delay=delay)


def transaction(query):
    """
    make query transactional
    :param query: the query
    """

    if iscoroutinefunction(query) or iscoroutine(query):
        return _AsyncTransactionScope(query)
    return _TransactionScope(query)


def _is_transaction_scope(connection):
    return getattr(connection, '_transaction_scope', False)


def _begin_transaction(connection):
    setattr(connection, '_transaction_scope', True)


def _close_transaction(connection):
    setattr(connection, '_transaction_scope', False)


class TransactionScope:
    """base class for transaction scope"""
    pass


class _AsyncTransactionScope(TransactionScope):
    """asynchronous transaction scope"""

    def __init__(self, query):
        self._query = query
        self._is_coroutine = True  # For iscoroutinefunction().

    @coroutine
    def __call__(self, connection):
        if _is_transaction_scope(connection):
            return (yield from self._query(connection))

        try:
            _begin_transaction(connection)
            r = yield from self._query(connection)
            yield from connection.commit()
            return r
        except:
            if connection.connected:
                yield from connection.rollback()
            raise
        finally:
            _close_transaction(connection)


class _TransactionScope(TransactionScope):
    """synchronous transaction scope"""

    def __init__(self, query):
        self._query = query

    def __call__(self, connection):
        if _is_transaction_scope(connection):
            return self._query(connection)

        try:
            _begin_transaction(connection)
            r = self._query(connection)
            connection.commit()
            return r
        except:
            if connection.connected:
                connection.rollback()
            raise
        finally:
            _close_transaction(connection)


class AsyncRetryable:
    """asynchronous retryable decorator"""
    def __init__(self, connection, count, delay, loop):
        import asyncio

        self._connection = connection
        self._count = count
        self._delay = delay
        self._loop = loop
        self._sleep = asyncio.sleep

    def execute(self, query):
        """execute query in current connection"""
        return self._execute(query, self._count - 1)

    @coroutine
    def _execute(self, query, retry_number):
        """internal method to execute query with retry-count check"""
        try:
            return (yield from self._connection.execute(query))
        except Exception as e:
            if retry_number <= 0 or not _websql.exceptions.is_retryable(e):
                raise
        yield from self._sleep(self._delay, loop=self._loop)
        return (yield from self._execute(query, retry_number - 1))


class _Retryable:
    """synchronous retryable decorator"""
    def __init__(self, connection, count, delay):
        import time
        self._connection = connection
        self._count = count
        self._delay = delay
        self._sleep = time.sleep

    def execute(self, query):
        """execute query in current connection"""
        return self._execute(query, self._count - 1)

    def _execute(self, query, retry_number):
        try:
            return self._connection.execute(query)
        except Exception as e:
            if retry_number <= 0 or not _websql.exceptions.is_retryable(e):
                raise

        self._sleep(self._delay)
        return self._execute(query, retry_number - 1)
