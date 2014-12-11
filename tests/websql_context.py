__author__ = "@bg"

from websql.cursors import INSERT_VALUES
from os import getenv
import _websql
import asyncio
import warnings
import weakref
import websql


warnings.filterwarnings('error')


class WebSQLContextBase:

    module = websql
    INSERT_VALUES = INSERT_VALUES
    errors = _websql.exceptions
    constants = _websql.constants

    connect_kwargs = {"database": getenv('WEBSQL_TEST_DATABASE', 'test'),
                      "user": getenv('WEBSQL_TEST_USER', 'root'),
                      "password": getenv('WEBSQL_TEST_PASSWORD', ''),
                      "charset": "utf8",
                      "sql_mode": "ANSI,STRICT_TRANS_TABLES,TRADITIONAL"}

    create_table_extra = "ENGINE=INNODB CHARACTER SET UTF8"
    rows = 10
    debug = False
    leak_test = False

    @staticmethod
    def identifier(ident):
        return '`%s`' % ident

    def __init__(self):
        self._connection = self.make_connection()
        self._cursor = None

    def make_connection(self):
        raise NotImplementedError


class WebSQLContext(WebSQLContextBase):
    def make_connection(self):
        return websql.connect(**self.connect_kwargs)

    def setup(self):
        self._cursor = self._connection.cursor()

    def tear_down(self):
        self._cursor.close()
        self._cursor = None

    def clean(self):
        self._connection.close()

    def connection(self):
        return self._connection

    def cursor(self):
        return self._cursor

    @staticmethod
    def unref(v):
        return v


class WebSQLAsyncContext(WebSQLContextBase):
    class WrapObject:
        def __init__(self, obj, context):
            super().__setattr__('obj', obj)
            super().__setattr__('context', context)

        def __getattr__(self, item):
            attr = getattr(self.obj, item)
            if callable(attr) and not isinstance(attr, type):
                return self.wrap_method(attr)
            return attr

        def __setattr__(self, key, value):
            setattr(self.obj, key, value)

        def wrap_method(self, func):
            def wrapper(*args, **kwargs):
                return self.context.unref(func(*args, **kwargs))
            return wrapper

    def __init__(self):
        self.loop = asyncio.new_event_loop()
        self._quard = weakref.finalize(self, lambda x: (x.stop(), x.close()), self.loop)

        super().__init__()

    def make_connection(self):
        return self.unref(websql.connect(nonblocking=True, loop=self.loop, **self.connect_kwargs))

    def setup(self):
        self._cursor = self._connection.cursor()

    def tear_down(self):
        self.unref(self._cursor.close())
        self._cursor = None

    def clean(self):
        self._connection.close()

    def connection(self):
        return self.WrapObject(self._connection, self)

    def cursor(self):
        return self.WrapObject(self._cursor, self)

    def unref(self, v):
        if isinstance(v, asyncio.Future) or asyncio.iscoroutine(v):
            return self.loop.run_until_complete(v)
        return v
