"""
WSQL
====
An asynchronous DB API v2.0 compatible interface to MySQL
---------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

__unittest = True

try:
    from _case import DatabaseTestCase
except ImportError:  # pragma: no cover
    from ._case import DatabaseTestCase

from datetime import timedelta
from time import time
import struct


class CapabilityTestCases(DatabaseTestCase):
    @classmethod
    def _setup_cls(cls):
        cls.BLOBText = ''.join([chr(i) for i in range(256)] * 100)
        cls.BLOBBinary = bytes(range(256)) * 16

    def _check_data_integrity(self, columns, generator, check=None):
        """check data integrity"""
        table = self._create_table(columns, generator)
        cursor = self._context.cursor()
        cursor.execute('select * from %s' % table)
        actual = cursor.fetchall()
        self.assertEqual(len(actual), self._context.rows)
        if check is None:
            check = self.assertEqual
        for i in range(self._context.rows):
            for j in range(len(columns)):
                check(actual[i][j], generator(i, j))

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
            v = (row * col) * 20 % 256
            if v > 127:  # pragma: no cover
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

    def test_float(self):
        """test_FLOAT"""
        def generator(row, col):
            if col == 0:
                return round(0.1 * row, 4)
            return round(0.1 * row / col, 4)
        self._check_data_integrity(('col1 FLOAT(7, 4)', 'col2 DOUBLE(7, 4)'), generator)

    def test_time(self):
        """test TIME"""
        def generator(row, col):
            if (row + col) % 2 == 0:
                return timedelta(0, (row + col) * -8000)
            return timedelta(0, (row + col) * 8000)

        self._check_data_integrity(('col1 TIME',), generator)

    def test_fractional_time(self):
        """test TIME"""
        def generator(row, col):
            if (row + col) % 2 == 0:
                return timedelta(0, (row + col) * -8000, microseconds=(row + col) * 100000)
            return timedelta(0, (row + col) * 8000, microseconds=(row + col) * 100000)

        if self._context.connection()._server_version >= (5, 6):
            self._check_data_integrity(('col1 TIME(2)',), generator)

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

        if self._context.connection()._server_version >= (5, 6):
            self._check_data_integrity(('col1 TIMESTAMP(2)',), generator)
        else:  # pragma: no cover
            self._check_data_integrity(('col1 TIMESTAMP',), generator)

    def test_bool(self):
        """test BOOLEAN"""
        def generator(row, col):
            return bool(row == col)
        self._check_data_integrity(('col1 BOOLEAN',), generator)

    def test_set(self):
        """test SET"""
        choises = ['a', 'b', 'c', 'd']

        def generator(row, col):
            if row == col:
                return set(choises)
            if row < col:  # pragma: no cover
                return set(choises[row:col])
            return set(choises[col:row])

        self._check_data_integrity(('col1 SET(' + ','.join(map(repr, choises)) + ')',), generator)

    def test_bit(self):
        """test BIT"""
        def generator(row, col):
            return struct.pack('>Q', row + col).lstrip(b'\x00')

        def check(actual, expected):
            if len(expected) < 8:
                expected = b'\x00' * (8 - len(expected)) + expected
            self.assertEqual(actual, struct.unpack('>Q', expected)[0])

        self._check_data_integrity(('col1 BIT(7)',), generator, check)

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
            self.assertEqual(self._context.constants.ER_NO_SUCH_TABLE, e.code)
        else:  # pragma: no cover
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
            if self._context.debug:  # pragma: no cover
                print(cursor.messages)
        except self._context.module.DataError:
            pass
        else:  # pragma: no cover
            self.fail("Over-long column did not generate warnings/exception with single insert")

        connection.rollback()

        try:
            for i in range(self._context.rows):
                data = []
                for j in range(len(columndefs)):
                    data.append(generator(i, j))
                cursor.execute(insert_statement, tuple(data))
        except Warning:
            if self._context.debug:   # pragma: no cover
                print(cursor.messages)
        except self._context.module.DataError:
            pass
        else:  # pragma: no cover
            self.fail("Over-long columns did not generate warnings/exception with execute()")

        connection.rollback()

        try:
            data = [[generator(i, j) for j in range(len(columndefs))] for i in range(self._context.rows)]
            cursor.executemany(insert_statement, data)
        except Warning:
            if self._context.debug:  # pragma: no cover
                print(cursor.messages)
        except self._context.module.DataError:
            pass
        else:  # pragma: no cover
            self.fail("Over-long columns did not generate warnings/exception with executemany()")

        connection.rollback()
