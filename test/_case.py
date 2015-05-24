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

from unittest import TestCase
from time import time


class DatabaseTestCase(TestCase):
    _context = None

    @classmethod
    def get_context(cls):
        pass  # pragma: no cover

    @classmethod
    def _setup_cls(cls):
        pass

    @classmethod
    def setUpClass(cls):
        cls._context = cls.get_context()
        cls._setup_cls()

    @classmethod
    def tearDownClass(cls):
        cls._context.clean()
        cls._context = None

    def setUp(self):
        self._context.setup()
        self.tables = []
        self.procedures = []

    def tearDown(self):
        cursor = self._context.cursor()
        connection = self._context.connection()

        if not self._context.debug:
            for table in self.tables:
                cursor.execute('drop table %s' % table)
            for proc in self.procedures:
                cursor.execute('drop procedure %s' % proc)
        connection.commit()
        self._context.tear_down()

    def unique_name(self, kind):
        """
        :param kind: table|procedure
        :type kind: str
        :return: the unique name
        :rtype: str
        """
        return ('%s_%s_%f' % (self.__class__.__name__.lower(), kind, time())).replace('.', '_')

    def _create_table(self, columns, generator, columns_extra=None):
        """ Create a table using a list of column definitions given in columns.
            generator must be a function taking arguments (row_number,
            col_number) returning a suitable data object for insertion
            into the table.
        """
        connection = self._context.connection()
        cursor = self._context.cursor()

        table = self.unique_name('table')
        if self._context.debug:  # pragma: no cover
            print('table name %s' % table)
        self.tables.append(table)

        if columns_extra is None:
            columns_extra = ''
        else:
            columns_extra = ',' + columns_extra

        cursor.execute('CREATE TABLE %s (%s %s) %s;' % (table, ','.join(columns), columns_extra, self._context.create_table_extra))
        if generator is not None:
            insert_statement = ('INSERT INTO %s VALUES (%s)' % (table, ','.join(['%s'] * len(columns))))
            data = [[generator(i, j) for j in range(len(columns))] for i in range(self._context.rows)]
            if self._context.debug:  # pragma: no cover
                print(data)
            cursor.executemany(insert_statement, data)
            connection.commit()
        return table

    def _create_procedure(self, args, body):
        connection = self._context.connection()
        cursor = self._context.cursor()

        procedure = self.unique_name('proc')
        self.procedures.append(procedure)

        cursor.execute("""
        CREATE PROCEDURE %s(%s)
        BEGIN
            %s
        END
        """ % (procedure, ','.join(args), body))

        connection.commit()
        return procedure
