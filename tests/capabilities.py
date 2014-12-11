""" Script to test database capabilities and the DB-API interface
    for functionality and memory leaks.

    Adapted from a script by M-A Lemburg.

 """
__unittest = True


from datetime import timedelta
from time import time
from tests.case import DatabaseTestCase


class CapabilityTestCases(DatabaseTestCase):
    @classmethod
    def get_context(cls):
        raise NotImplementedError

    @classmethod
    def _setup_cls(cls):
        cls.BLOBText = ''.join([chr(i) for i in range(256)] * 100)
        cls.BLOBBinary = bytes(range(256)) * 16

    def _create_table(self, columns, generator):
        """ Create a table using a list of column definitions given in columns.
            generator must be a function taking arguments (row_number,
            col_number) returning a suitable data object for insertion
            into the table.
        """
        connection = self._context.connection()
        cursor = self._context.cursor()

        table = ("table_%d_%f" % (id(self), time())).replace('.', '_')
        if self._context.debug:
            print('table name %s' % table)
        self.tables.append(table)

        cursor.execute('CREATE TABLE %s (%s) %s;' % (table, ','.join(columns), self._context.create_table_extra))
        if generator is not None:
            insert_statement = ('INSERT INTO %s VALUES (%s)' % (table, ','.join(['%s'] * len(columns))))
            data = [[generator(i, j) for j in range(len(columns))] for i in range(self._context.rows)]
            if self._context.debug:
                print(data)
            cursor.executemany(insert_statement, data)
            connection.commit()
        return table

    def _create_procedure(self, args, body):
        connection = self._context.connection()
        cursor = self._context.cursor()

        procedure = ('procedure_%d_%f' % (id(self), time())).replace('.', '_')
        self.procedures.append(procedure)

        cursor.execute("""
        CREATE PROCEDURE %s(%s)
        BEGIN
            %s
        END
        """ % (procedure, ','.join(args), body))

        connection.commit()
        return procedure

    def _check_data_integrity(self, columns, generator):
        """check data integrity"""
        table = self._create_table(columns, generator)
        cursor = self._context.cursor()
        cursor.execute('select * from %s' % table)
        actual = cursor.fetchall()
        self.assertEqual(len(actual), self._context.rows)
        for i in range(self._context.rows):
            for j in range(len(columns)):
                self.assertEqual(actual[i][j], generator(i, j))

    def test_blob(self):
        """test BLOB"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBBinary  # 'BLOB\000Binary ' * 1024
        self._check_data_integrity(('col1 INT', 'col2 BLOB'), generator)

    def test_long_byte(self):
        """test LONG_BYTE"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBBinary  # 'BLOB\000Binary ' * 1024
        self._check_data_integrity(('col1 INT', 'col2 LONG BYTE'), generator)

    def test_char(self):
        """test CHAR"""
        def generator(row, col):
            return ('%i' % ((row + col) % 10)) * 255
        self._check_data_integrity(('col1 char(255)', 'col2 char(255)'), generator)

    def test_small_char(self):
        """test SMALL CHAR"""
        def generator(row, col):
            i = (row * col + 62) % 256
            if i == 62:
                return ''
            if i == 63:
                return None
            return chr(i)
        self._check_data_integrity(('col1 char(1)', 'col2 char(1)'), generator)

    def test_text(self):
        """test TEXT"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBText
        self._check_data_integrity(('col1 INT', 'col2 TEXT'), generator)

    def test_tiny_int(self):
        """test TINYINT"""
        def generator(row, col):
            v = (row * col) % 256
            if v > 127:
                v -= 256
            return v

        self._check_data_integrity(('col1 TINYINT',), generator)

    def test_int(self):
        """test INT"""
        def generator(row, col):
            return row * col
        self._check_data_integrity(('col1 INT',), generator)

    def test_decimal(self):
        """test DECIMAL"""
        def generator(row, col):
            from decimal import Decimal
            return Decimal("%d.%02d" % (row, col))
        self._check_data_integrity(('col1 DECIMAL(5,2)',), generator)

    def test_long(self):
        """test_LONG"""
        def generator(row, col):
            if col == 0:
                return row
            else:
                return self.BLOBText
        self._check_data_integrity(('col1 INT', 'col2 LONG'), generator)

    def test_time(self):
        """test TIME"""
        def generator(row, col):
            return timedelta(0, (row + col) * 8000)

        self._check_data_integrity(('col1 TIME',), generator)

    def test_date(self):
        """test DATE"""
        ticks = time()

        def generator(row, col):
            return self._context.module.DateFromTicks(ticks + row * 86400 - col * 1313)
        self._check_data_integrity(('col1 DATE',), generator)

    def test_datetime(self):
        """test DATETIME"""
        ticks = time()

        def generator(row, col):
            return self._context.module.TimestampFromTicks(ticks + row * 86400 - col * 1313)
        self._check_data_integrity(('col1 DATETIME',), generator)

    def test_timestamp(self):
        """test TIMESTAMP"""
        ticks = time()

        def generator(row, col):
            return self._context.module.TimestampFromTicks(ticks + row * 86400 - col * 1313)

        self._check_data_integrity(('col1 TIMESTAMP',), generator)

    def test_fractional_timestamp(self):
        """test fractional TIMESTAMP"""
        ticks = time()

        def generator(row, col):
            return self._context.module.TimestampFromTicks(ticks + row * 86400 - col * 1313 + row * 0.7 * col / 3.0)
        self._check_data_integrity(('col1 TIMESTAMP',), generator)

    def test_stored_procedures(self):
        """test stored procedures call"""

        dictionary = 'ash birch cedar larch pine'.split()

        def generator(row, col):
            if col == 0:
                return row
            return dictionary[row]

        self._context.rows = len(dictionary)
        table = self._create_table(('pos INT', 'tree CHAR(20)'), generator)
        cursor = self._context.cursor()
        procedure = self._create_procedure(('IN t VARCHAR(255)',), 'SELECT pos FROM %s WHERE tree = t;' % table)
        cursor.callproc(procedure, ('larch',))
        rows = cursor.fetchall()
        self.assertEqual(1, len(rows))
        self.assertEqual(3, rows[0][0])
        self.assertIsNone(cursor.nextset())

    def test_bug_2671682(self):
        cursor = self._context.cursor()
        try:
            cursor.execute("describe some_non_existent_table")
        except self._context.module.ProgrammingError as e:
            self.assertEqual(self._context.constants.ER_NO_SUCH_TABLE, e.args[0])
        else:
            self.fail('ProgrammingError should be raised!')

    def test_insert_values(self):
        """test_INSERT_VALUES"""
        insert_values = self._context.INSERT_VALUES

        query = b"""INSERT FOO (a, b, c) VALUES (%s, %s, %s)"""
        matched = insert_values.match(query)
        self.assertTrue(matched)
        start = matched.group('start')
        end = matched.group('end')
        values = matched.group('values')
        self.assertEqual(b"""INSERT FOO (a, b, c) VALUES """, start)
        self.assertEqual(b"(%s, %s, %s)", values)
        self.assertEqual(b"", end)

    def test_ping(self):
        connection = self._context.connection()
        connection.ping()

    def test_quote(self):
        self.assertEqual(b"'foo'", self._context.connection().quote("foo"))

    def test_transactions(self):
        def generator(row, col):
            if col == 0:
                return row
            else:
                return ('%i' % (row % 10)) * 255

        table = self._create_table(('col1 INT', 'col2 VARCHAR(255)'), generator)
        cursor = self._context.cursor()
        connection = self._context.connection()
        cursor.execute('delete from %s where col1=%%s' % table, (0,))
        cursor.execute('select col1 from %s where col1=%%s' % table, (0,))
        l = cursor.fetchall()
        self.assertEqual(0, len(l), "DELETE didn't work")
        connection.rollback()
        cursor.execute('select col1 from %s where col1=%%s' % table, (0,))
        l = cursor.fetchall()
        self.assertEqual(1, len(l), "ROLLBACK didn't work")

    def test_truncation(self):
        columndefs = ('col1 INT', 'col2 VARCHAR(255)')

        def generator(row, col):
            if col == 0:
                return row
            else:
                return ('%i' % (row % 10),) * ((255 - self._context.rows // 2) + row)

        table = self._create_table(columndefs, None)
        insert_statement = ('INSERT INTO %s VALUES (%s)' % (table, ','.join(['%s'] * len(columndefs))))
        connection = self._context.connection()
        cursor = self._context.cursor()

        try:
            cursor.execute(insert_statement, (0, '0' * 256))
        except Warning:
            if self._context.debug:
                print(cursor.messages)
        except self._context.module.DataError:
            pass
        else:
            self.fail("Over-long column did not generate warnings/exception with single insert")

        connection.rollback()

        try:
            for i in range(self._context.rows):
                data = []
                for j in range(len(columndefs)):
                    data.append(generator(i, j))
                cursor.execute(insert_statement, tuple(data))
        except Warning:
            if self._context.debug:
                print(cursor.messages)
        except self._context.module.DataError:
            pass
        else:
            self.fail("Over-long columns did not generate warnings/exception with execute()")

        connection.rollback()

        try:
            data = [[generator(i, j) for j in range(len(columndefs))] for i in range(self._context.rows)]
            cursor.executemany(insert_statement, data)
        except Warning:
            if self._context.debug:
                print(cursor.messages)
        except self._context.module.DataError:
            pass
        else:
            self.fail("Over-long columns did not generate warnings/exception with executemany()")

        connection.rollback()
