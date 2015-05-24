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

__author__ = "@bg"

from websql.cursors import INSERT_VALUES
from websql.connections import UNSET
from os import getenv
import _websql
import asyncio
import warnings
import websql

warnings.filterwarnings('error')


class WebSQLSetupBase:

    module = websql
    INSERT_VALUES = INSERT_VALUES
    errors = _websql.exceptions
    constants = _websql.constants

    connect_kwargs = {"host": getenv('WEBSQL_TEST_HOST', '127.0.0.1'),
                      "database": getenv('WEBSQL_TEST_DATABASE', 'test'),
                      "user": getenv('WEBSQL_TEST_USER', 'root'),
                      "password": getenv('WEBSQL_TEST_PASSWORD', ''),
                      "charset": "utf8",
                      "sql_mode": "ANSI,STRICT_TRANS_TABLES,TRADITIONAL"}

    create_table_extra = "ENGINE=INNODB CHARACTER SET UTF8 COLLATE utf8_unicode_ci"
    rows = 10
    debug = False
    leak_test = False


class WebSQLSetup(WebSQLSetupBase):
    loop = UNSET

    @staticmethod
    def decorator(func):
        """function decorate policy"""
        return func

    @staticmethod
    def iscoroutine(_):  # pragma: no cover
        return False

    def connect(self):
        """
        :return: new connection to database
        """
        return websql.connect(**self.connect_kwargs)

    @staticmethod
    def wait(ref):
        """
        :param ref: reference to object
        :return: get object from reference
        """
        return ref

    @staticmethod
    def wrap(obj):
        """
        :param obj:
        :return: reference to object
        """
        return obj

    @staticmethod
    def call_and_wait(func, *args, **kwargs):
        """
        Call the method and wait result
        :param func: callable object
        :param args: positional arguments
        :param kwargs: keyword arguments
        :return: the method result
        """
        return func(*args, **kwargs)

    @staticmethod
    def clean():
        """
        clean context after complete work
        """
        pass


class WebSQLSetupAsync(WebSQLSetupBase):

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
                return self.context.wait(func(*args, **kwargs))
            return wrapper

    def __init__(self):
        super().__init__()
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        self.loop = loop
        self.loop.set_debug(self.debug)

    @staticmethod
    def decorator(func):
        return asyncio.coroutine(func)

    @staticmethod
    def iscoroutine(func):
        return asyncio.iscoroutine(func) or asyncio.iscoroutinefunction(func)

    def clean(self):
        """
        clean context after complete work
        """
        self.loop.stop()
        self.loop.close()
        self.loop = None
        asyncio.set_event_loop(None)

    def connect(self):
        """
        :return: Future for connection
        """
        return websql.connect(loop=self.loop, **self.connect_kwargs)

    def wait(self, obj):
        """
        :param obj: Future or coroutine
        :return: the real value
        """
        if isinstance(obj, asyncio.Future) or asyncio.iscoroutine(obj):
            return self.loop.run_until_complete(obj)
        return obj

    def wrap(self, obj):
        """
        :param obj: object that can return future
        :return: the wrapped object, that always resolve value of Future
        """
        return self.WrapObject(obj, self)

    def call_and_wait(self, func, *args, **kwargs):
        """
        Call the method and wait result
        :param callable: callable object
        :param args: positional arguments
        :param kwargs: keyword arguments
        :return: the method result
        """
        return self.wait(func(*args, **kwargs))


class WebSQLContextBase:
    def __init__(self, setup):
        self._setup = setup
        self._connection = setup.wait(setup.connect())
        self._cursor = None

    def __getattr__(self, item):
        return getattr(self._setup, item)

    def make_connection(self):
        """
        make a new connection
        :return: new connection object
        """
        return self._setup.wait(self._setup.connect())

    def setup(self):
        """
        initialise test-case
        """
        self._cursor = self._connection.cursor()

    def tear_down(self):
        """
        clean after test completed
        """
        self._cursor.close()
        self._cursor = None

    def clean(self):
        """
        clean after suite completed
        """
        self._connection.close()
        self._setup.clean()
        self._setup = None

    def connection(self):
        """
        :return: current connection
        """
        return self._setup.wrap(self._connection)

    def cursor(self):
        """
        :return: current cursor
        """
        return self._setup.wrap(self._cursor)

    def wait(self, ref):
        """
        :param ref: reference object
        :return: the value of reference
        """
        return self._setup.wait(ref)


class WebSQLContext(WebSQLContextBase):
    def __init__(self):
        super().__init__(WebSQLSetup())


class WebSQLAsyncContext(WebSQLContextBase):
    def __init__(self):
        super().__init__(WebSQLSetupAsync())
