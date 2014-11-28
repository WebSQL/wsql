from tests import capabilities
import unittest
import MySQLdb
import warnings
import os

warnings.filterwarnings('error')

config_file = os.path.join(os.path.dirname(__file__), 'my.cnf')


class TestWebSQLCapabilities(capabilities.DatabaseTest):

    db_module = MySQLdb
    connect_args = ()
    connect_kwargs = dict(database='test', read_default_file=config_file, read_default_group='websql',
                          user='root', charset='utf8', sql_mode="ANSI,STRICT_TRANS_TABLES,TRADITIONAL")

    create_table_extra = "ENGINE=INNODB CHARACTER SET UTF8"
    leak_test = False
    
    def quote_identifier(self, identifier):
        return "`%s`" % identifier

    def test_time(self):
        """test_TIME"""
        from datetime import timedelta

        def generator(row, col):
            return timedelta(0, (row + col) * 8000)
        self.check_data_integrity(('col1 TIME',), generator)

    def test_tiny_int(self):
        """test_TINYINT"""
        def generator(row, col):
            v = (row * col) % 256
            if v > 127:
                v -= 256
            return v

        self.check_data_integrity(('col1 TINYINT',), generator)
        
    def test_stored_procedures(self):
        db = self.connection
        c = self.cursor
        try:
            self.create_table(('pos INT', 'tree CHAR(20)'))
            c.executemany("INSERT INTO %s (pos,tree) VALUES (%%s,%%s)" % self.table,
                          list(enumerate('ash birch cedar larch pine'.split())))
            db.commit()

            c.execute("""
            CREATE PROCEDURE test_sp(IN t VARCHAR(255))
            BEGIN
                SELECT pos FROM %s WHERE tree = t;
            END
            """ % self.table)
            db.commit()
    
            c.callproc('test_sp', ('larch',))
            rows = c.fetchall()
            self.assertEqual(len(rows), 1)
            self.assertEqual(rows[0][0], 3)
            c.nextset()
        finally:
            c.execute("DROP PROCEDURE IF EXISTS test_sp")
            c.execute('drop table %s' % self.table)

    def test_small_char(self):
        """test_small_"""
        def generator(row, col):
            i = (row * col + 62) % 256
            if i == 62:
                return ''
            if i == 63:
                return None
            return chr(i)
        self.check_data_integrity(('col1 char(1)', 'col2 char(1)'), generator)

    def test_bug_2671682(self):
        from MySQLdb._websql import constants
        try:
            self.cursor.execute("describe some_non_existent_table")
        except self.connection.ProgrammingError as err:
            self.assertEqual(constants.ER_NO_SUCH_TABLE, err.args[0])
    
    def test_INSERT_VALUES(self):
        """test_INSERT_VALUES"""
        from MySQLdb.cursors import INSERT_VALUES
        query = b"""INSERT FOO (a, b, c) VALUES (%s, %s, %s)"""
        matched = INSERT_VALUES.match(query)
        self.assertTrue(matched)
        start = matched.group('start')
        end = matched.group('end')
        values = matched.group('values')
        self.assertEqual(b"""INSERT FOO (a, b, c) VALUES """, start)
        self.assertEqual(b"(%s, %s, %s)", values)
        self.assertEqual(b"", end)
        
    def test_ping(self):
        self.connection.ping()

    def test_quote_string(self):
        self.assertEqual(b"'foo'", self.connection.quote("foo"))
        
    def test_blob(self):
        """test_BLOB"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBBinary  # 'BLOB\000Binary ' * 1024
        self.check_data_integrity(('col1 INT', 'col2 BLOB'), generator)

    def test_long(self):
        """test_LONG"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBText
        self.check_data_integrity(('col1 INT', 'col2 LONG'), generator)

    def test_text(self):
        """test_TEXT"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBText  # 'BLOB Text ' * 1024
        self.check_data_integrity(('col1 INT', 'col2 TEXT'), generator)



if __name__ == '__main__':
    if TestWebSQLCapabilities.leak_test:
        import gc
        gc.enable()
        gc.set_debug(gc.DEBUG_LEAK)
    unittest.main()
