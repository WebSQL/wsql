"""
WebSQL Scope Objects
--------------------

This module implements scope objects for connections.
"""

from asyncio import iscoroutine, iscoroutinefunction, coroutine, get_event_loop
import _websql

__all__ = ["transaction", "retryable"]


def nonblocking(cls):
    cls._nonblocking = True
    return cls


def is_nonblocking(cls):
    return getattr(cls, '_nonblocking', False)


def retryable(connection, count=5, delay=1, loop=None):
    """
    Make connection execute retryable
    :param connection: the connection object
    :param count: the maximum number of retries
    :param delay: the delay between retries
    :param loop: optional parameter, event loop of asynchronous method
    :return: the Connection, that has retryable method execute
    """
    count = count or 1
    if is_nonblocking(connection):
        return RetryableAsync(connection, loop=loop, count=count, delay=delay)
    return _RetryableSync(connection, count=count, delay=delay)


def transaction(handler):
    """
    make handler transactional
    :param handler: the handler
    """

    if iscoroutinefunction(handler) or iscoroutine(handler):
        return _TransactionScopeAsync(handler)
    return _TransactionScopeSync(handler)


def _is_transaction_scope(connection):
    return getattr(connection, '_transaction_scope', False)


def _begin_transaction(connection):
    setattr(connection, '_transaction_scope', True)


def _close_transaction(connection):
    setattr(connection, '_transaction_scope', False)


class TransactionScope:
    """base class for transaction scope"""
    pass


class _TransactionScopeAsync(TransactionScope):
    """asynchronous transaction scope"""

    def __init__(self, handler):
        self._handler = handler
        self._is_coroutine = True  # For iscoroutinefunction().

    @coroutine
    def __call__(self, connection):
        if _is_transaction_scope(connection):
            return (yield from self._handler(connection))

        try:
            _begin_transaction(connection)
            r = yield from self._handler(connection)
            yield from connection.commit()
            return r
        except:
            if connection.connected:
                yield from connection.rollback()
            raise
        finally:
            _close_transaction(connection)


class _TransactionScopeSync(TransactionScope):
    """synchronous transaction scope"""

    def __init__(self, handler):
        self._handler = handler

    def __call__(self, connection):
        if _is_transaction_scope(connection):
            return self._handler(connection)

        try:
            _begin_transaction(connection)
            r = self._handler(connection)
            connection.commit()
            return r
        except:
            if connection.connected:
                connection.rollback()
            raise
        finally:
            _close_transaction(connection)


class RetryableAsync:
    """asynchronous retryable decorator"""
    def __init__(self, connection, count, delay, loop):
        import asyncio

        self._connection = connection
        self._count = count
        self._delay = delay
        self._loop = loop or get_event_loop()
        self._sleep = asyncio.sleep

    def execute(self, handler):
        """execute handler in current connection"""
        return self._execute(handler, self._count - 1)

    @coroutine
    def _execute(self, handler, retry_number):
        """internal method to execute handler with retry-count check"""
        try:
            return (yield from self._connection.execute(handler))
        except Exception as e:
            if retry_number <= 0 or not _websql.exceptions.is_retryable(e):
                raise
        yield from self._sleep(self._delay, loop=self._loop)
        return (yield from self._execute(handler, retry_number - 1))


class _RetryableSync:
    """synchronous retryable decorator"""
    def __init__(self, connection, count, delay):
        import time
        self._connection = connection
        self._count = count
        self._delay = delay
        self._sleep = time.sleep

    def execute(self, handler):
        """execute handler in current connection"""
        return self._execute(handler, self._count - 1)

    def _execute(self, handler, retry_number):
        try:
            return self._connection.execute(handler)
        except Exception as e:
            if retry_number <= 0 or not _websql.exceptions.is_retryable(e):
                raise

        self._sleep(self._delay)
        return self._execute(handler, retry_number - 1)
