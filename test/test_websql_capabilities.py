__author__ = "@bg"

try:
    from _capabilities import CapabilityTestCases
    from _websql_context import WebSQLContext, WebSQLAsyncContext
except ImportError:  # pragma: no cover
    from ._capabilities import CapabilityTestCases
    from ._websql_context import WebSQLContext, WebSQLAsyncContext


class TestWebSQLCapabilities(CapabilityTestCases):
    @classmethod
    def get_context(cls):
        return WebSQLContext()


class TesWebSQLAsyncWebSQLAsyncCapabilities(CapabilityTestCases):
    @classmethod
    def get_context(cls):
        return WebSQLAsyncContext()

del CapabilityTestCases
