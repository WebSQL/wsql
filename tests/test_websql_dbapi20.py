__author__ = "@bg"

from tests.dbapi20 import DatabaseAPI20TestCases
from tests.websql_context import WebSQLContext, WebSQLAsyncContext


class TestWebSQLDBAPI20(DatabaseAPI20TestCases):
    @classmethod
    def get_context(cls):
        return WebSQLContext()


class TestWebSQLAsyncDBAPI20(DatabaseAPI20TestCases):
    @classmethod
    def get_context(cls):
        return WebSQLAsyncContext()

del DatabaseAPI20TestCases
