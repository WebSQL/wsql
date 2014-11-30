"""
@file
@brief    
@details  Copyright (c) 2001-2014 Acronis
@author   Bulat Gaifullin (bulat.gaifullin@acronis.com)
@since    $Id: $
"""

from _websql import constants as __constatnts

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

STRING = DBAPISet([__constatnts.FIELD_TYPE_ENUM, __constatnts.FIELD_TYPE_STRING, __constatnts.FIELD_TYPE_VAR_STRING])
BINARY = DBAPISet([__constatnts.FIELD_TYPE_BLOB, __constatnts.FIELD_TYPE_LONG_BLOB,
                   __constatnts.FIELD_TYPE_MEDIUM_BLOB, __constatnts.FIELD_TYPE_TINY_BLOB])

NUMBER = DBAPISet([__constatnts.FIELD_TYPE_DECIMAL, __constatnts.FIELD_TYPE_DOUBLE, __constatnts.FIELD_TYPE_FLOAT,
                   __constatnts.FIELD_TYPE_INT24, __constatnts.FIELD_TYPE_LONG, __constatnts.FIELD_TYPE_LONGLONG,
                   __constatnts.FIELD_TYPE_TINY, __constatnts.FIELD_TYPE_YEAR])

DATE = DBAPISet([__constatnts.FIELD_TYPE_DATE, __constatnts.FIELD_TYPE_NEWDATE])
TIME = DBAPISet([__constatnts.FIELD_TYPE_TIME])
TIMESTAMP = DBAPISet([__constatnts.FIELD_TYPE_TIMESTAMP, __constatnts.FIELD_TYPE_DATETIME])
DATETIME = TIMESTAMP
ROWID = DBAPISet()
