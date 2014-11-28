import unittest
import MySQLdb
from MySQLdb import _websql


class TestDBAPISet(unittest.TestCase):
    def test_set_equality(self):
        self.assertEqual(MySQLdb.STRING, MySQLdb.STRING)

    def test_set_inequality(self):
        self.assertTrue(MySQLdb.STRING, MySQLdb.NUMBER)

    def test_set_equality_membership(self):
        self.assertEqual(_websql.constants.FIELD_TYPE_VAR_STRING, MySQLdb.STRING)

    def test_set_inequality_membership(self):
        self.assertTrue(_websql.constants.FIELD_TYPE_DATE != MySQLdb.STRING)


class CoreModule(unittest.TestCase):
    """Core _mysql module features."""

    def test_NULL(self):
        """Should have a NULL constant."""
        self.assertEqual(_websql.NULL, 'NULL')

    def test_version(self):
        """Version information sanity."""
        self.assertTrue(isinstance(_websql.__version__, str))

        self.assertTrue(isinstance(_websql.version_info, tuple))
        self.assertEqual(len(_websql.version_info), 5)

    def test_client_info(self):
        self.assertTrue(isinstance(_websql.get_client_info(), str))

    def test_thread_safe(self):
        self.assertTrue(isinstance(_websql.thread_safe(), int))


class CoreAPI(unittest.TestCase):
    """Test _mysql interaction internals."""

    def setUp(self):
        self.conn = _websql.connect(db='test', read_default_file="~/.my.cnf")

    def tearDown(self):
        self.conn.close()

    def test_thread_id(self):
        tid = self.conn.thread_id()
        self.assertIsInstance(tid, int, "thread_id didn't return an int.")
        self.assertRaises(TypeError, self.conn.thread_id, ('evil',), "thread_id shouldn't accept arguments.")

    def test_affected_rows(self):
        self.assertEqual(0, self.conn.affected_rows(), "Should return 0 before we do anything.")

    def test_charset_name(self):
        self.assertIsInstance(self.conn.character_set_name(), str, "Should return a string.")

    def test_host_info(self):
        self.assertIsInstance(self.conn.get_host_info(), str, "Should return a string.")

    def test_proto_info(self):
        self.assertIsInstance(self.conn.get_proto_info(), int, "Should return an int.")

    def test_server_info(self):
        self.assertIsInstance(self.conn.get_server_info(), str, "Should return an str.")

