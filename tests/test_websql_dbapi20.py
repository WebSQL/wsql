from tests import dbapi20
import unittest
import MySQLdb


class TestWebSQLApi(dbapi20.DatabaseAPI20Test):
    driver = MySQLdb
    connect_args = ()
    connect_kw_args = dict(db='test',
                           read_default_file='~/.my.cnf',
                           charset='utf8',
                           sql_mode="ANSI,STRICT_TRANS_TABLES,TRADITIONAL")

    def test_setoutputsize(self):
        pass

    def test_setoutputsize_basic(self):
        pass

    def test_fetchall(self):
        con = self._connect()
        try:
            cur = con.cursor()
            self.assertRaises(self.driver.Error, cur.fetchall)

            self.execute_ddl1(cur)
            for sql in self._populate():
                cur.execute(sql)

            cur.execute('select name from %sbooze' % self.table_prefix)
            rows = cur.fetchall()
            self.assertIn(cur.rowcount, (-1, len(self.samples)))
            rows = sorted(r[0] for r in rows)
            self.assertEqual(rows, self.samples, 'cursor.fetchall retrieved incorrect rows')
            rows = cur.fetchall()
            self.assertEqual(
                0, len(rows),
                'cursor.fetchall should return an empty list if called '
                'after the whole result set has been fetched'
            )
            self.assertIn(cur.rowcount, (-1, len(self.samples)))

            self.execute_ddl2(cur)
            cur.execute('select name from %sbarflys' % self.table_prefix)
            rows = cur.fetchall()
            self.assertIn(cur.rowcount, (-1, 0))
            self.assertEqual(
                len(rows), 0,
                'cursor.fetchall should return an empty list if '
                'a select query returns no rows'
            )
            
        finally:
            con.close()
                
    def test_fetchone(self):
        con = self._connect()
        try:
            cur = con.cursor()
            self.assertRaises(self.driver.Error, cur.fetchone)
            self.execute_ddl1(cur)
            cur.execute('select name from %sbooze' % self.table_prefix)
            self.assertEqual(
                cur.fetchone(), None,
                'cursor.fetchone should return None if a query retrieves '
                'no rows'
            )
            self.assertIn(cur.rowcount, (-1, 0))

            # cursor.fetchone should raise an Error if called after
            # executing a query that cannnot return rows
            cur.execute("insert into %sbooze values ('Victoria Bitter')" % self.table_prefix)
            cur.execute('select name from %sbooze' % self.table_prefix)
            r = cur.fetchone()
            self.assertEqual(
                1, len(r),
                'cursor.fetchone should have retrieved a single row'
            )
            self.assertEqual(
                r[0], 'Victoria Bitter',
                'cursor.fetchone retrieved incorrect data'
            )
            self.assertIn(cur.rowcount, (-1, 1))
        finally:
            con.close()

    # Same complaint as for fetchall and fetchone
    def test_rowcount(self):
        con = self._connect()
        try:
            cur = con.cursor()
            self.execute_ddl1(cur)
            cur.execute("insert into %sbooze values ('Victoria Bitter')" % self.table_prefix)
            cur.execute("select name from %sbooze" % self.table_prefix)
            self.assertIn(
                cur.rowcount, (-1, 1),
                'cursor.rowcount should == number of rows returned, or '
                'set to -1 after executing a select statement'
            )
            self.execute_ddl2(cur)
        finally:
            con.close()

    def test_callproc(self):
        pass

    def setup_next_set(self, cur):
        """
            Should create a procedure called deleteme
            that returns two result sets, first the 
            number of rows in booze then "name from booze
        """
        sql = """
           create procedure deleteme()
           begin
               select count(*) from %(tp)sbooze;
               select name from %(tp)sbooze;
           end
        """ % dict(tp=self.table_prefix)
        cur.execute(sql)

    def cleanup_next_set(self, cur):
        """If cleaning up is needed after nextSetTest"""
        cur.execute("drop procedure deleteme")


if __name__ == '__main__':
    unittest.main()