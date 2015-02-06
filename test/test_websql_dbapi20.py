"""test DB-API 2.0 compatibility"""

__author__ = "@bg"

try:
    from _dbapi20 import DatabaseAPI20TestCases
    from _websql_context import WebSQLContext, WebSQLAsyncContext
except ImportError:  # pragma: no cover
    from ._dbapi20 import DatabaseAPI20TestCases
    from ._websql_context import WebSQLContext, WebSQLAsyncContext


class TestWebSQLDBAPI20(DatabaseAPI20TestCases):
    @classmethod
    def get_context(cls):
        return WebSQLContext()


class TestWebSQLAsyncDBAPI20(DatabaseAPI20TestCases):
    @classmethod
    def get_context(cls):
        return WebSQLAsyncContext()

del DatabaseAPI20TestCases
