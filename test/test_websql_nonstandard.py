"""test non-standard function of package"""

try:
    from _case import DatabaseTestCase, TestCase
    from _websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase
except ImportError:  # pragma: no cover
    from ._case import DatabaseTestCase, TestCase
    from ._websql_context import WebSQLSetup, WebSQLSetupAsync, WebSQLContextBase

from websql import converters
import websql
import _websql
import warnings
import types


class TestDBAPISet(TestCase):
    def test_set_equality(self):
        self.assertEqual(websql.STRING, websql.STRING)

    def test_set_inequality(self):
        self.assertNotEqual(websql.STRING, websql.NUMBER)

    def test_set_equality_membership(self):
        self.assertEqual(_websql.constants.FIELD_TYPE_VAR_STRING, websql.STRING)

    def test_set_inequality_membership(self):
        self.assertNotEqual(_websql.constants.FIELD_TYPE_DATE, websql.STRING)


class TestConverters(TestCase):
    def test_tuple_row_decoder(self):
        """test format row as tuple"""
        rows = list(range(4))
        decoders = (lambda x: x for _ in range(len(rows)))
        self.assertIsNone(converters.tuple_row_decoder(decoders, None, None))
        result = converters.tuple_row_decoder(decoders, None, rows)
        self.assertEqual(tuple(rows), result)

    def test_iter_row_decoder(self):
        """test format row as generator"""
        rows = list(range(4))
        decoders = (lambda x: x for _ in range(len(rows)))
        self.assertIsNone(converters.iter_row_decoder(decoders, None, None))
        result = converters.iter_row_decoder(decoders, None, rows)
        self.assertIsInstance(result, types.GeneratorType)
        self.assertEqual(tuple(rows), tuple(result))

    def test_dict_row_decoder(self):
        """test format row as dict"""
        rows = list(range(4))

        names = ("a", "b.c", "d.e.f.g", "i.k.l.m.n")
        decoders = (lambda x: x for _ in range(len(rows)))
        self.assertIsNone(converters.dict_row_decoder(decoders, None, None))
        result = converters.dict_row_decoder(decoders, names, rows)
        self.assertEqual({"a": 0, "b": {"c": 1}, "d": {"e": {"f": {"g": 2}}}, "i": {"k": {"l": {"m": {"n": 3}}}}}, result)

    def test_get_codec(self):
        """test get_codec method"""
        self.assertRaisesRegex(WebSQLSetup.errors.NotSupportedError, "could not encode as SQL", converters.get_codec, WebSQLSetup.errors, None, ())
        self.assertRaisesRegex(WebSQLSetup.errors.NotSupportedError, "could not encode as SQL", converters.get_codec, WebSQLSetup.errors, None, (lambda x, y: None,))
        self.assertEqual("a", converters.get_codec(None, None, (lambda x, y: "a",)))


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
        return WebSQLContextBase(WebSQLSetup())


class TestCoreAsyncApi(CoreAPI):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetupAsync())

del CoreAPI


class TestCursorBase(DatabaseTestCase):
    def __check_warning(self, message, func, *args):
        with warnings.catch_warnings(record=True) as records:
            try:
                func(*args)
            except Warning as w:  # pragma: no cover
                records.append(w)
            if message is not None:
                self.assertEqual(1, len(records))
                self.assertIn(message, str(records[0]))
            else:
                self.assertEqual(0, len(records))

    def test_warning_if_not_closed(self):
        """test unclosed cursor generate warning on delete"""
        cursor = self._context.wrap(self._context.connection().cursor())
        try:
            cursor.execute('select 1;')
            self.__check_warning('unclosed', cursor.__del__)
        finally:
            cursor.close()

    def test_clean_resources_on_close(self):
        """test free resources when close cursor"""
        cursor = self._context.wrap(self._context.connection().cursor())
        cursor.execute('select 1; select 2')
        cursor.close()
        self.__check_warning(None, cursor.__del__)

    def test_warnings_check(self):
        """test warning_check method of cursor"""
        cursor = self._context.wrap(self._context.connection().cursor())
        try:
            self.__check_warning('Unknown table', cursor.execute, 'drop table if exists %s' % self.unique_name('table'))
        finally:
            cursor.close()


class TestCursor(TestCursorBase):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetup())

    def test_context_manager(self):
        # in case if cursor will not closed warning will be raised
        with self._context.connection().cursor() as cursor:
            cursor.execute('select 1;')


class TestCursorAsync(TestCursorBase):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetupAsync())

del TestCursorBase
