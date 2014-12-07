from tests.case import DatabaseTestCase, TestCase
from tests.websql_context import WebSQLContext, WebSQLAsyncContext
import websql
import _websql


class TestDBAPISet(TestCase):
    def test_set_equality(self):
        self.assertEqual(websql.STRING, websql.STRING)

    def test_set_inequality(self):
        self.assertNotEqual(websql.STRING, websql.NUMBER)

    def test_set_equality_membership(self):
        self.assertEqual(_websql.constants.FIELD_TYPE_VAR_STRING, websql.STRING)

    def test_set_inequality_membership(self):
        self.assertNotEqual(_websql.constants.FIELD_TYPE_DATE, websql.STRING)


class TestCoreModule(TestCase):
    """Core c-module features."""

    def test_NULL(self):
        """Should have a NULL constant."""
        self.assertEqual(_websql.constants.NULL, 'NULL')

    def test_version(self):
        """Version information sanity."""
        self.assertIsInstance(_websql.__version__, str)

        self.assertIsInstance(_websql.version_info, tuple)
        self.assertEqual(5, len(_websql.version_info))

    def test_client_info(self):
        self.assertIsInstance(_websql.get_client_info(), str)

    def test_thread_safe(self):
        self.assertIsInstance(_websql.thread_safe(), int)


class CoreAPI(DatabaseTestCase):
    """Test _mysql interaction internals."""
    @classmethod
    def get_context(cls):
        raise NotImplementedError

    def test_thread_id(self):
        connection = self._context.connection()
        self.assertIsInstance(connection.thread_id, int, "thread_id didn't return an int.")

    def test_affected_rows(self):
        connection = self._context.connection()
        self.assertEqual(0, connection.affected_rows, "Should return 0 before we do anything.")

    def test_charset_name(self):
        connection = self._context.connection()
        self.assertIsInstance(connection.charset, str, "Should return a string.")

    def test_host_info(self):
        connection = self._context.connection()
        self.assertIsInstance(connection.host_info, str, "Should return a string.")

    def test_proto_info(self):
        connection = self._context.connection()
        self.assertIsInstance(connection.proto_info, int, "Should return an int.")

    def test_server_info(self):
        connection = self._context.connection()
        self.assertIsInstance(connection.server_info, str, "Should return an str.")


class TestCoreApi(CoreAPI):
    @classmethod
    def get_context(cls):
        return WebSQLContext()


class TestCoreAsyncApi(CoreAPI):
    @classmethod
    def get_context(cls):
        return WebSQLAsyncContext()

del CoreAPI

if __name__ == '__main__':
    import unittest
    unittest.main()
