"""
WebSQL Connection Pool
-------------------

This module implements connections pools for WebSQL.
"""

from asyncio import iscoroutine, iscoroutinefunction, coroutine

__all__ = ["transaction"]


def transaction(handler):
    """
    make handler as a single transaction
    :param handler: the handler
    """
    @coroutine
    def wrap_coroutine(connection):
        if is_transaction_scope(connection):
            return (yield from handler(connection))

        try:
            begin_transaction(connection)
            r = yield from handler(connection)
            yield from connection.commit()
            return r
        except:
            if not connection.closed:
                yield from connection.rollback()
            raise
        finally:
            close_transaction(connection)

    def wrap(connection):
        if is_transaction_scope(connection):
            return handler(connection)

        try:
            begin_transaction(connection)
            r = handler(connection)
            connection.commit()
            return r
        except:
            if not connection.closed:
                connection.rollback()
            raise
        finally:
            close_transaction(connection)

    if iscoroutinefunction(handler) or iscoroutine(handler):
        return wrap_coroutine
    return wrap


def is_transaction_scope(connection):
    return getattr(connection, '_transaction_scope', False)


def begin_transaction(connection):
    setattr(connection, '_transaction_scope', True)


def close_transaction(connection):
    setattr(connection, '_transaction_scope', False)
