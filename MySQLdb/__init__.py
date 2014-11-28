"""
MySQLdb
=======
A DB API v2.0 compatible interface to MySQL
-------------------------------------------

This package is a wrapper around _websql, which mostly implements the
MySQL C API.

See the C API specification and the MySQL documentation for more info
on other items.

For information on the DB API, see PEP-249.

For information on how MySQLdb handles type conversion, see the
MySQLdb.converters module.
"""

from .release import __version__, version_info, __author__
from .connections import *
from .times import *
from ._types import *

__all__ = [
    'connect', 'apilevel', 'threadsafety', 'paramstyle',
    'Warning', 'Error', 'InterfaceError', 'DatabaseError', 'DataError', 'OperationalError',
    'IntegrityError', 'InternalError', 'ProgrammingError', 'NotSupportedError',
    'Date', 'Time', 'Timestamp', 'DateFromTicks', 'TimeFromTicks', 'TimestampFromTicks',
    'Binary', 'STRING', 'BINARY', 'NUMBER', 'DATETIME', 'ROWID'
]

threadsafety = 1
apilevel = "2.0"
paramstyle = "format"
