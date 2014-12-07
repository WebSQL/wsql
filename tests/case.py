"""base test case class"""

__author__ = "@bg"
__unittest = True

from unittest import TestCase
from time import time


class DatabaseTestCase(TestCase):
    _context = None

    @classmethod
    def get_context(cls):
        raise NotImplementedError

    @classmethod
    def setUpClass(cls):
        cls._context = cls.get_context()

    @classmethod
    def tearDownClass(cls):
        cls._context.clean()

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

    def _unique_name(self, kind):
        return ('%s_%s_%f' % (self.__class__.__name__.lower(), kind, time())).replace('.', '_')

    def _create_table(self, columns, generator):
        """ Create a table using a list of column definitions given in columns.
            generator must be a function taking arguments (row_number,
            col_number) returning a suitable data object for insertion
            into the table.
        """
        connection = self._context.connection()
        cursor = self._context.cursor()

        table = self._unique_name('table')
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

        procedure = self._unique_name('proc')
        self.procedures.append(procedure)

        cursor.execute("""
        CREATE PROCEDURE %s(%s)
        BEGIN
            %s
        END
        """ % (procedure, ','.join(args), body))

        connection.commit()
        return procedure