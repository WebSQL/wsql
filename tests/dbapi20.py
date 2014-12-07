"""
    Python DB API 2.0 driver compliance unit test suite.
    This software is Public Domain and may be used without restrictions.

    Now we have booze and barflies entering the discussion, plus rumours of
    DBAs on drugs... and I won't tell you what flashes through my mind each
    time I read the subject line with 'Anal Compliance' in it.  All around
    this is turning out to be a thoroughly unwholesome unit test."
    -- Ian Bicking
"""

__author__ = 'Stuart Bishop <zen@shangri-la.dropbear.id.au>'
__unittest = True

from tests.case import DatabaseTestCase
import time


class DatabaseAPI20TestCases(DatabaseTestCase):
    """
        Test a database self._context.module for DB API 2.0 compatibility.
        This implementation tests Gadfly, but the TestCase
        is structured so that other self._context.modules can subclass this
        test case to ensure compiliance with the DB-API. It is
        expected that this TestCase may be expanded in the future
        if ambiguities or edge conditions are discovered.

        The 'Optional Extensions' are not yet being tested.

        self._context.modules should subclass this test, overriding setUp, tearDown,
        self._context.module, connect_args and connect_kw_args. Class specification
        should be as follows:

        import dbapi20
        class mytest(dbapi20.DatabaseAPI20Test):
           [...]

        Don't 'import DatabaseAPI20Test from dbapi20', or you will
        confuse the unit tester - just 'import dbapi20'.
    """

    # ddl1 = 'create table %sbooze (name varchar(20))' % table_prefix
    # ddl2 = 'create table %sbarflys (name varchar(20))' % table_prefix
    # xddl1 = 'drop table %sbooze' % table_prefix
    # xddl2 = 'drop table %sbarflys' % table_prefix

    # lowerfunc = 'lower'  # Name of stored procedure to convert string->lowercase

    @classmethod
    def get_context(cls):
        raise NotImplementedError

    def setUp(self):
        super().setUp()
        self.cursors = []

    def tearDown(self):
        for cursor in self.cursors:
            self._context.unref(cursor.close())
        super().tearDown()

    def test_connect(self):
        connection = self._context.make_connection()
        connection.close()

    def test_apilevel(self):
        try:
            # Must exist
            apilevel = self._context.module.apilevel
            # Must equal 2.0
            self.assertEqual(apilevel, '2.0')
        except AttributeError:
            self.fail("Driver doesn't define apilevel")

    def test_threadsafety(self):
        try:
            # Must exist
            threadsafety = self._context.module.threadsafety
            # Must be a valid value
            self.assertIn(threadsafety, list(range(4)))
        except AttributeError:
            self.fail("Driver doesn't define threadsafety")

    def test_exceptions(self):
        """test_Exceptions"""
        driver = self._context.module
        self.assertTrue(hasattr(driver, 'Warning'))
        self.assertTrue(issubclass(driver.Error, driver.StandardError))
        self.assertTrue(issubclass(driver.InterfaceError, driver.Error))
        self.assertTrue(issubclass(driver.DatabaseError, driver.Error))
        self.assertTrue(issubclass(driver.OperationalError, driver.Error))
        self.assertTrue(issubclass(driver.IntegrityError, driver.Error))
        self.assertTrue(issubclass(driver.InternalError, driver.Error))
        self.assertTrue(issubclass(driver.ProgrammingError, driver.Error))
        self.assertTrue(issubclass(driver.NotSupportedError, driver.Error))

    def test_exceptions_as_connection_attributes(self):
        """test_ExceptionsAsConnectionAttributes"""
        # OPTIONAL EXTENSION
        # Test for the optional DB API 2.0 extension, where the exceptions
        # are exposed as attributes on the Connection object
        # I figure this optional extension will be implemented by any
        # driver author who is using this test suite, so it is enabled
        # by default.
        connection = self._context.connection()
        driver = self._context.module
        self.assertIs(connection.Warning, driver.Warning)
        self.assertIs(connection.Error, driver.Error)
        self.assertIs(connection.InterfaceError, driver.InterfaceError)
        self.assertIs(connection.DatabaseError, driver.DatabaseError)
        self.assertIs(connection.OperationalError, driver.OperationalError)
        self.assertIs(connection.IntegrityError, driver.IntegrityError)
        self.assertIs(connection.InternalError, driver.InternalError)
        self.assertIs(connection.ProgrammingError, driver.ProgrammingError)
        self.assertIs(connection.NotSupportedError, driver.NotSupportedError)

    def test_commit(self):
        connection = self._context.connection()
        # Commit must work, even if it doesn't do anything
        connection.commit()

    def test_rollback(self):
        connection = self._context.connection()
        # If rollback is defined, it should either work or throw
        # the documented exception
        if hasattr(connection, 'rollback'):
            try:
                connection.rollback()
            except self._context.module.NotSupportedError:
                pass

    def test_cursor(self):
        connection = self._context.connection()
        connection.cursor()
        self._context.unref(connection.cursor().close())

    def test_cursor_isolation(self):
        connection = self._context.connection()
        # Make sure cursors created from the same connection have
        # the documented transaction isolation level
        cur1 = connection.cursor()
        self.cursors.append(cur1)
        cur2 = connection.cursor()
        self.cursors.append(cur2)
        table = self._unique_name('table')
        self.tables.append(table)
        self._context.unref(cur1.execute('create table %s (name varchar(20))' % table))
        self._context.unref(cur1.execute("insert into %s values ('Victoria Bitter')" % table))
        self._context.unref(cur2.execute("select name from %s" % table))
        rows = self._context.unref(cur2.fetchall())
        self.assertEqual(len(rows), 1)
        self.assertEqual(len(rows[0]), 1)
        self.assertEqual(rows[0][0], 'Victoria Bitter')

    def test_description(self):
        cursor = self._context.cursor()

        table = self._create_table(('name varchar(20)',), None)

        self.assertEqual(
            cursor.description, None,
            'cursor.description should be none after executing a '
            'statement that can return no rows (such as DDL)'
        )

        cursor.execute('select name from %s' % table)
        self.assertEqual(
            1, len(cursor.description),
            'cursor.description describes too many columns'
        )
        self.assertEqual(
            7, len(cursor.description[0]),
            'cursor.description[x] tuples must have 7 elements'
        )
        self.assertEqual(
            'name', cursor.description[0][0].lower(),
            'cursor.description[x][0] must return column name'
        )
        self.assertEqual(
            self._context.module.STRING, cursor.description[0][1],
            'cursor.description[x][1] must return column type. Got %r' % cursor.description[0][1]
        )
        # clean result
        cursor.fetchall()

        # Make sure self.description gets reset
        self._create_table(('name varchar(20)',), None)
        self.assertEqual(
            cursor.description, None,
            'cursor.description not being set to None when executing '
            'no-result statements (eg. DDL)'
        )

    def test_rowcount(self):
        cursor = self._context.cursor()
        table = self._create_table(('name varchar(20)',), None)

        self.assertEqual(
            cursor.rowcount, -1,
            'cursor.rowcount should be -1 after executing no-result '
            'statements'
        )
        cursor.execute("insert into %s values ('Victoria Bitter')" % table)
        self.assertIn(
            cursor.rowcount, (-1, 1),
            'cursor.rowcount should == number of rows inserted, or '
            'set to -1 after executing an insert statement'
        )
        cursor.execute("select name from %s" % table)
        self.assertIn(
            cursor.rowcount, (-1, 1),
            'cursor.rowcount should == number of rows returned, or '
            'set to -1 after executing a select statement'
        )

        # clear cursor
        cursor.nextset()

        self._create_table(('name varchar(20)',), None)
        self.assertEqual(
            -1, cursor.rowcount,
            'cursor.rowcount not being reset to -1 after executing '
            'no-result statements'
        )

    def test_callproc(self):
        cursor = self._context.cursor()
        procedure = self._create_procedure(('s varchar(255)',), 'select lower(s);')
        r = cursor.callproc(procedure, ('FOO',))
        self.assertEqual(1, len(r))
        self.assertEqual('FOO', r[0])
        r = cursor.fetchall()
        self.assertIsNone(cursor.nextset())
        self.assertEqual(1, len(r), 'callproc produced no result set')
        self.assertEqual(1, len(r[0]), 'callproc produced invalid result set')
        self.assertEqual('foo', r[0][0], 'callproc produced invalid results')

    def test_close(self):
        connection = self._context.make_connection()
        try:
            cursor = connection.cursor()
        finally:
            connection.close()

        # cursor.execute should raise an Error if called after connection
        # closed
        self.assertRaises(self._context.module.Error, lambda x: self._context.unref(cursor.execute(x)), 'show tables')

        # connection.commit should raise an Error if called after connection'
        # closed.'
        self.assertRaises(self._context.module.Error, lambda: self._context.unref(connection.commit()))

        # connection.close should raise an Error if called more than once
        self.assertRaises(self._context.module.Error, connection.close)

    PARAM_FORMAT = {
        'qmark': ('insert into %s values (?)', tuple),
        'numeric': ('insert into %s values (:1)', tuple),
        'format': ('insert into %s values (%%s)', tuple),
        'named': ('insert into %s values (:beer)', lambda x: dict(zip(('beer',), x))),
        'pyformat': ('insert into %s values (%%(beer)s)', lambda x: dict(zip(('beer',), x))),
    }

    def test_paramstyle(self):
        self.assertTrue(hasattr(self._context.module, 'paramstyle'))
        paramstyle = self._context.module.paramstyle
        self.assertIn(paramstyle, self.PARAM_FORMAT)

    def test_execute(self):
        cursor = self._context.cursor()
        table = self._create_table(('name varchar(20)',), None)
        cursor.execute("insert into %s values ('Victoria Bitter')" % table)
        self.assertIn(cursor.rowcount, (-1, 1))
        try:
            template, converter = self.PARAM_FORMAT[self._context.module.paramstyle]
        except KeyError:
            self.fail('Invalid paramstyle')

        cursor.execute(template % table, converter(("Cooper's",)))
        self.assertIn(cursor.rowcount, (-1, 1))

        cursor.execute('select name from %s' % table)
        res = cursor.fetchall()
        self.assertEqual(2, len(res), 'cursor.fetchall returned too few rows')
        beers = [res[0][0], res[1][0]]
        beers.sort()
        self.assertEqual(
            beers[0], "Cooper's",
            'cursor.fetchall retrieved incorrect data, or data inserted '
            'incorrectly'
        )
        self.assertEqual(
            beers[1], "Victoria Bitter",
            'cursor.fetchall retrieved incorrect data, or data inserted '
            'incorrectly'
        )

    def test_executemany(self):
        cursor = self._context.cursor()
        table = self._create_table(('name varchar(20)',), None)

        try:
            template, converter = self.PARAM_FORMAT[self._context.module.paramstyle]
        except KeyError:
            self.fail('Invalid paramstyle')

        cursor.executemany(template % table, (converter(x) for x in (("Cooper's",), ("Boag's",))))

        self.assertIn(
            cursor.rowcount, (-1, 2),
            'insert using cursor.executemany set cursor.rowcount to '
            'incorrect value %r' % cursor.rowcount
        )
        cursor.execute('select name from %s' % table)
        res = cursor.fetchall()
        self.assertEqual(
            2, len(res),
            'cursor.fetchall retrieved incorrect number of rows'
        )
        beers = [res[0][0], res[1][0]]
        beers.sort()
        self.assertEqual("Boag's", beers[0], 'incorrect data retrieved')
        self.assertEqual("Cooper's", beers[1], 'incorrect data retrieved')

    def test_fetchone(self):
        cursor = self._context.cursor()

        # cursor.fetchone should raise an Error if called before
        # executing a select-type query
        self.assertRaises(self._context.module.Error, cursor.fetchone)

        # cursor.fetchone should raise an Error if called after
        # executing a query that can not return rows
        table = self._create_table(('name varchar(20)',), None)
        self.assertRaises(self._context.module.Error, cursor.fetchone)

        cursor.execute('select name from %s' % table)
        self.assertIsNone(
            cursor.fetchone(),
            'cursor.fetchone should return None if a query retrieves '
            'no rows'
        )
        self.assertIn(cursor.rowcount, (-1, 0))

        # cursor.fetchone should raise an Error if called after
        # executing a query that can not return rows
        cursor.execute("insert into %s values ('Victoria Bitter')" % table)
        self.assertRaises(self._context.module.Error, cursor.fetchone)

        cursor.execute('select name from %s' % table)
        r = cursor.fetchone()
        self.assertEqual(
            1, len(r),
            'cursor.fetchone should have retrieved a single row'
        )
        self.assertEqual(
            r[0], 'Victoria Bitter',
            'cursor.fetchone retrieved incorrect data'
        )
        self.assertIsNone(
            cursor.fetchone(),
            'cursor.fetchone should return None if no more rows available'
        )
        self.assertIn(cursor.rowcount, (-1, 1))

    SAMPLES = [
        'Carlton Cold',
        'Carlton Draft',
        'Mountain Goat',
        'Redback',
        'Victoria Bitter',
        'XXXX'
    ]

    def test_fetchmany(self):
        cursor = self._context.cursor()
        # cursor.fetchmany should raise an Error if called without
        # issuing a query
        self.assertRaises(self._context.module.Error, cursor.fetchmany, 4)
        self._context.rows = len(self.SAMPLES)
        table = self._create_table(('name varchar(20)',), lambda x, y: y == 0 and self.SAMPLES[x])

        cursor.execute('select name from %s' % table)

        rows = cursor.fetchmany()
        self.assertEqual(
            1, len(rows),
            'cursor.fetchmany retrieved incorrect number of rows, '
            'default of arraysize is one.'
        )
        cursor.arraysize = 10
        rows = cursor.fetchmany(3)  # Should get 3 rows
        self.assertEqual(
            3, len(rows),
            'cursor.fetchmany retrieved incorrect number of rows'
        )
        rows = cursor.fetchmany(4)  # Should get 2 more
        self.assertEqual(
            len(self.SAMPLES) - 4, len(rows),
            'cursor.fetchmany retrieved incorrect number of rows'
        )
        rows = cursor.fetchmany(4)  # Should be an empty sequence
        self.assertEqual(
            0, len(rows),
            'cursor.fetchmany should return an empty sequence after '
            'results are exhausted'
        )

        self.assertIn(cursor.rowcount, (-1, 6))

        # Same as above, using cursor.arraysize
        cursor.arraysize = 4
        cursor.execute('select name from %s' % table)
        r = cursor.fetchmany()  # Should get 4 rows
        self.assertEqual(
            4, len(r),
            'cursor.arraysize not being honoured by fetchmany'
        )
        rows = cursor.fetchmany()  # Should get 2 more
        self.assertEqual(2, len(rows))
        rows = cursor.fetchmany()  # Should be an empty sequence
        self.assertEqual(0, len(rows))
        self.assertIn(cursor.rowcount, (-1, 6))

        cursor.arraysize = 6
        cursor.execute('select name from %s' % table)
        rows = cursor.fetchmany()  # Should get all rows
        self.assertIn(cursor.rowcount, (-1, 6))
        self.assertEqual(6, len(rows))
        rows = [r[0] for r in rows]
        rows.sort()

        # Make sure we get the right data back out
        for i in range(0, 6):
            self.assertEqual(
                rows[i], self.SAMPLES[i],
                'incorrect data retrieved by cursor.fetchmany'
            )

        rows = cursor.fetchmany()  # Should return an empty list
        self.assertEqual(
            0, len(rows),
            'cursor.fetchmany should return an empty sequence if '
            'called after the whole result set has been fetched'
        )
        self.assertIn(cursor.rowcount, (-1, 6))

        # clear cursor
        cursor.nextset()

        table = self._create_table(('name varchar(20)',), None)
        cursor.execute('select name from %s' % table)
        r = cursor.fetchmany()  # Should get empty sequence
        self.assertEqual(
            0, len(r),
            'cursor.fetchmany should return an empty sequence if '
            'query retrieved no rows'
        )
        self.assertIn(cursor.rowcount, (-1, 0))

    def test_fetchall(self):
        cursor = self._context.cursor()
        # cursor.fetchall should raise an Error if called
        # without executing a query that may return rows (such
        # as a select)
        self.assertRaises(self._context.module.Error, cursor.fetchall)

        self._context.rows = len(self.SAMPLES)
        table = self._create_table(('name varchar(20)',), lambda x, y: y == 0 and self.SAMPLES[x])

        # cursor.fetchall should raise an Error if called
        # after executing a a statement that cannot return rows
        self.assertRaises(self._context.module.Error, cursor.fetchall)

        cursor.execute('select name from %s' % table)
        rows = cursor.fetchall()
        self.assertIn(cursor.rowcount, (-1, len(self.SAMPLES)))
        self.assertEqual(
            len(self.SAMPLES), len(rows),
            'cursor.fetchall did not retrieve all rows'
        )
        rows = sorted(r[0] for r in rows)
        for i in range(0, len(self.SAMPLES)):
            self.assertEqual(rows[i], self.SAMPLES[i], 'cursor.fetchall retrieved incorrect rows')

        rows = cursor.fetchall()
        self.assertEqual(
            0, len(rows),
            'cursor.fetchall should return an empty list if called '
            'after the whole result set has been fetched'
        )
        self.assertIn(cursor.rowcount, (-1, len(self.SAMPLES)))

        table = self._create_table(('name varchar(20)',), None)
        cursor.execute('select name from %s' % table)
        rows = cursor.fetchall()
        self.assertIn(cursor.rowcount, (-1, 0))
        self.assertEqual(
            0, len(rows),
            'cursor.fetchall should return an empty list if '
            'a select query returns no rows'
        )

    def test_mixedfetch(self):
        cursor = self._context.cursor()
        self._context.rows = len(self.SAMPLES)
        table = self._create_table(('name varchar(20)',), lambda x, y: y == 0 and self.SAMPLES[x])

        cursor.execute('select name from %s' % table)
        rows1 = cursor.fetchone()
        rows23 = cursor.fetchmany(2)
        rows4 = cursor.fetchone()
        rows56 = cursor.fetchall()
        self.assertIn(cursor.rowcount, (-1, 6))
        self.assertEqual(
            2, len(rows23),
            'fetchmany returned incorrect number of rows'
        )
        self.assertEqual(
            2, len(rows56),
            'fetchall returned incorrect number of rows'
        )

        rows = [rows1[0]]
        rows.extend([rows23[0][0], rows23[1][0]])
        rows.append(rows4[0])
        rows.extend([rows56[0][0], rows56[1][0]])
        rows.sort()
        for i in range(0, len(self.SAMPLES)):
            self.assertEqual(
                self.SAMPLES[i], rows[i],
                'incorrect data retrieved or inserted'
            )

    def test_nextset(self):
        cursor = self._context.cursor()
        self._context.rows = len(self.SAMPLES)
        table = self._create_table(('name varchar(20)',), lambda x, y: y == 0 and self.SAMPLES[x])

        procedure = self._create_procedure((), 'select count(*) from {0}; select name from {0};'.format(table))
        cursor.callproc(procedure)
        numberofrows = cursor.fetchone()
        self.assertEqual(len(self.SAMPLES), numberofrows[0])
        self.assertTrue(cursor.nextset())
        names = cursor.fetchall()
        self.assertEqual(len(self.SAMPLES), len(names))
        self.assertIsNone(cursor.nextset(), 'No more return sets, should return None')

    def test_arraysize(self):
        cursor = self._context.cursor()
        self.assertTrue(hasattr(cursor, 'arraysize'), 'cursor.arraysize must be defined')

    def test_none(self):
        """test None"""
        cursor = self._context.cursor()
        self._context.rows = 1
        table = self._create_table(('name varchar(20)',), lambda x, y: None)
        cursor.execute('select name from %s' % table)
        rows = cursor.fetchall()
        self.assertEqual(1, len(rows))
        self.assertEqual(1, len(rows[0]))
        self.assertIsNone(rows[0][0], 'NULL value not returned as None')

    def test_date(self):
        """test_Date"""
        d1 = self._context.module.Date(2002, 12, 25)
        d2 = self._context.module.DateFromTicks(time.mktime((2002, 12, 25, 0, 0, 0, 0, 0, 0)))
        self.assertEqual(d1, d2)

    def test_time(self):
        """test_Time"""
        t1 = self._context.module.Time(13, 45, 30)
        t2 = self._context.module.TimeFromTicks(time.mktime((2001, 1, 1, 13, 45, 30, 0, 0, 0)))
        self.assertEqual(t1, t2)

    def test_timestamp(self):
        """test_Timestamp"""
        t1 = self._context.module.Timestamp(2002, 12, 25, 13, 45, 30)
        t2 = self._context.module.TimestampFromTicks(time.mktime((2002, 12, 25, 13, 45, 30, 0, 0, 0)))
        self.assertEqual(t1, t2)

    def test_binary(self):
        """test_Binary"""
        self.assertIsInstance(self._context.module.Binary(b'Something'), self._context.module.Binary)
        self.assertIsInstance(self._context.module.Binary(b''), self._context.module.Binary)

    def test_string(self):
        """test_STRING"""
        self.assertTrue(hasattr(self._context.module, 'STRING'), 'module.STRING must be defined')

    def test_binary2(self):
        """test_BINARY"""
        self.assertTrue(hasattr(self._context.module, 'BINARY'), 'module.BINARY must be defined.')

    def test_number(self):
        """test_NUMBER"""
        self.assertTrue(hasattr(self._context.module, 'NUMBER'), 'module.NUMBER must be defined.')

    def test_datetime(self):
        """test_DATETIME"""
        self.assertTrue(hasattr(self._context.module, 'DATETIME'), 'module.DATETIME must be defined.')

    def test_rowid(self):
        """test_ROWID"""
        self.assertTrue(hasattr(self._context.module, 'ROWID'), 'module.ROWID must be defined.')
