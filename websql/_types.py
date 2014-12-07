"""
WebSQL types
-------------------

This module implements connections for MySQLdb. Presently there is
only one class: Connection. Others are unlikely. However, you might
want to make your own subclasses. In most cases, you will probably
override Connection.default_cursor with a non-standard Cursor class.

"""

import _websql

__all__ = ["STRING", "BINARY", "NUMBER", "DATE", "DATETIME",
           "TIME", "TIMESTAMP", "ROWID", "Binary"]


class DBAPISet(frozenset):
    """
        A special type of set for which A == x is True if A is a
        DBAPISet and x is a member of that set.
    """
    def __eq__(self, other):
        if isinstance(other, DBAPISet):
            return not self.difference(other)
        return other in self


Binary = bytes

STRING = DBAPISet([_websql.constants.FIELD_TYPE_ENUM, _websql.constants.FIELD_TYPE_STRING, _websql.constants.FIELD_TYPE_VAR_STRING])
BINARY = DBAPISet([_websql.constants.FIELD_TYPE_BLOB, _websql.constants.FIELD_TYPE_LONG_BLOB,
                   _websql.constants.FIELD_TYPE_MEDIUM_BLOB, _websql.constants.FIELD_TYPE_TINY_BLOB])

NUMBER = DBAPISet([_websql.constants.FIELD_TYPE_DECIMAL, _websql.constants.FIELD_TYPE_DOUBLE, _websql.constants.FIELD_TYPE_FLOAT,
                   _websql.constants.FIELD_TYPE_INT24, _websql.constants.FIELD_TYPE_LONG, _websql.constants.FIELD_TYPE_LONGLONG,
                   _websql.constants.FIELD_TYPE_TINY, _websql.constants.FIELD_TYPE_YEAR])

DATE = DBAPISet([_websql.constants.FIELD_TYPE_DATE, _websql.constants.FIELD_TYPE_NEWDATE])
TIME = DBAPISet([_websql.constants.FIELD_TYPE_TIME])
TIMESTAMP = DBAPISet([_websql.constants.FIELD_TYPE_TIMESTAMP, _websql.constants.FIELD_TYPE_DATETIME])
DATETIME = TIMESTAMP
ROWID = DBAPISet()
