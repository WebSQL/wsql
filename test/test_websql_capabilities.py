__author__ = "@bg"

from tests.capabilities import CapabilityTestCases
from tests.websql_context import WebSQLContext, WebSQLAsyncContext


class TestWebSQLCapabilities(CapabilityTestCases):
    @classmethod
    def get_context(cls):
        return WebSQLContext()


class TesWebSQLAsyncWebSQLAsyncCapabilities(CapabilityTestCases):
    @classmethod
    def get_context(cls):
        return WebSQLAsyncContext()

del CapabilityTestCases
