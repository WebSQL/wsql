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

import _wsql

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

STRING = DBAPISet([_wsql.constants.FIELD_TYPE_ENUM, _wsql.constants.FIELD_TYPE_STRING, _wsql.constants.FIELD_TYPE_VAR_STRING])
BINARY = DBAPISet([_wsql.constants.FIELD_TYPE_BLOB, _wsql.constants.FIELD_TYPE_LONG_BLOB,
                   _wsql.constants.FIELD_TYPE_MEDIUM_BLOB, _wsql.constants.FIELD_TYPE_TINY_BLOB])

NUMBER = DBAPISet([_wsql.constants.FIELD_TYPE_DECIMAL, _wsql.constants.FIELD_TYPE_DOUBLE, _wsql.constants.FIELD_TYPE_FLOAT,
                   _wsql.constants.FIELD_TYPE_INT24, _wsql.constants.FIELD_TYPE_LONG, _wsql.constants.FIELD_TYPE_LONGLONG,
                   _wsql.constants.FIELD_TYPE_TINY, _wsql.constants.FIELD_TYPE_YEAR])

DATE = DBAPISet([_wsql.constants.FIELD_TYPE_DATE, _wsql.constants.FIELD_TYPE_NEWDATE])
TIME = DBAPISet([_wsql.constants.FIELD_TYPE_TIME])
TIMESTAMP = DBAPISet([_wsql.constants.FIELD_TYPE_TIMESTAMP, _wsql.constants.FIELD_TYPE_DATETIME])
DATETIME = TIMESTAMP
ROWID = DBAPISet()
