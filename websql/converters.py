"""
MySQLdb type conversion module
------------------------------



"""

from _websql import constants

from decimal import Decimal
from math import modf
import datetime
import struct


NULL = constants.NULL


def bool_to_sql(_, value):
    """Convert a Python bool to an SQL literal."""
    return int(value)


def set_to_sql(connection, value):
    """Convert a Python set to an SQL literal."""
    return connection.string_literal(','.join(value)).decode(connection.charset)


def sql_to_set(value):
    """Convert MySQL SET column to Python set."""
    return {i for i in value.split(',') if i}


def object_to_sql(connection, obj):
    """Convert something into a string via str().
    The result will not be quoted."""
    return str_to_sql(connection, str(obj))


def str_to_sql(connection, value):
    """Convert a string object to a sql string literal by using connection encoding."""
    charset = connection.charset
    return connection.quote(value.encode(charset))


def bytes_to_sql(connection, value):
    """Convert a bytes object to a sql string literal"""
    return connection.quote(value)


def int_to_sql(_, value):
    """Convert a bytes object to a sql int literal"""
    return value


def float_to_sql(_, value):
    """Convert float to sql literal"""
    return '%.15g' % value


def decimal_to_sql(_, value):
    """Convert Decimal to sql literal"""
    return value


def sql_to_decimal(value):
    """Convert SQL literal to Decimal"""
    return Decimal(value.decode('ascii'))


def none_to_sql(*_):
    """Convert None to NULL."""
    return NULL


def timedelta_to_sql(_, obj):
    """Convert timedelta to a SQL literal"""
    seconds = int(obj.seconds) % 60
    minutes = int(obj.seconds / 60) % 60
    hours = int(obj.seconds / 3600) % 24
    if obj.microseconds:
        return ("'%02d:%02d:%02d.%06d'" % (hours, minutes, seconds, obj.microseconds)).encode('ascii')
    return ("'%02d:%02d:%02d'" % (hours, minutes, seconds)).encode('ascii')


def datetime_to_sql(_, obj):
    """Convert a datetime to a SQL literal"""
    return obj.strftime("'%Y-%m-%d %H:%M:%S'").encode('ascii')


def sql_to_date(obj):
    """Convert a SQL literal to date"""
    return datetime.date(*map(int, obj.split(b'-')))


def sql_to_datetime(obj):
    """Convert a SQL literal to datetime"""
    date_, time_ = obj.split(b' ')
    if len(time_) > 8:
        hms, mcs = time_.split(b'.')
        mcs = int(mcs.ljust(6, b'0'))
    else:
        hms = time_
        mcs = 0

    parts = [int(i) for i in date_.split(b'-')]
    parts.extend(map(int, hms.split(b':')))
    parts.append(mcs)
    return datetime.datetime(*parts)


def sql_to_timedelta(obj):
    """Convert a SQL literal to timedelta"""
    hours, minutes, seconds = obj.split(b':')
    delta = datetime.timedelta(
        hours=int(hours),
        minutes=int(minutes),
        seconds=int(seconds),
        microseconds=int(modf(float(seconds))[0] * 1000000))
    if delta.seconds < 0:
        return -delta
    return delta


def sql_to_bit(value):
    """Returns BIT columntype as integer"""
    if len(value) < 8:
        value = b'\x00' * (8 - len(value)) + value
    return struct.unpack('>Q', value)[0]


def any_to_sql(connection, obj):
    """Convert any object to sql literal."""
    return str_to_sql(connection, str(obj))


def none_if_null(func):
    if func is None:
        return func

    def _none_if_null(value):
        return value if value is None else func(value)
    _none_if_null.__name__ = func.__name__ + "_or_None_if_NULL"
    return _none_if_null


int_or_None_if_NULL = none_if_null(int)
float_or_None_if_NULL = none_if_null(float)
decimal_or_None_if_NULL = none_if_null(sql_to_decimal)
set_or_None_if_NULL = none_if_null(sql_to_set)
datetime_or_None_if_NULL = none_if_null(sql_to_datetime)
date_or_None_if_NULL = none_if_null(sql_to_date)
timedelta_or_None_if_NULL = none_if_null(sql_to_timedelta)
bytes_or_None_if_NULL = none_if_null(lambda x: x)
bit_or_None_if_NULL = none_if_null(sql_to_bit)


simple_type_encoders = {
    bool: bool_to_sql,
    bytes: bytes_to_sql,
    datetime.datetime: datetime_to_sql,
    datetime.timedelta: timedelta_to_sql,
    float: float_to_sql,
    int: int_to_sql,
    set: set_to_sql,
    str: str_to_sql,
    type(None): none_to_sql
}

# This is for MySQL column types that can be converted directly
# into Python types without having to look at metadata (flags,
# character sets, etc.). This should always be used as the last
# resort.
simple_field_decoders = {
    constants.FIELD_TYPE_TINY_BLOB: bytes_or_None_if_NULL,
    constants.FIELD_TYPE_MEDIUM_BLOB: bytes_or_None_if_NULL,
    constants.FIELD_TYPE_LONG_BLOB: bytes_or_None_if_NULL,

    constants.FIELD_TYPE_TINY: int_or_None_if_NULL,
    constants.FIELD_TYPE_SHORT: int_or_None_if_NULL,
    constants.FIELD_TYPE_LONG: int_or_None_if_NULL,
    constants.FIELD_TYPE_INT24: int_or_None_if_NULL,
    constants.FIELD_TYPE_LONGLONG: int_or_None_if_NULL,

    constants.FIELD_TYPE_FLOAT: float_or_None_if_NULL,
    constants.FIELD_TYPE_DOUBLE: float_or_None_if_NULL,

    constants.FIELD_TYPE_DECIMAL: decimal_or_None_if_NULL,
    constants.FIELD_TYPE_NEWDECIMAL: decimal_or_None_if_NULL,

    constants.FIELD_TYPE_YEAR: int_or_None_if_NULL,

    constants.FIELD_TYPE_TIMESTAMP: datetime_or_None_if_NULL,
    constants.FIELD_TYPE_DATETIME: datetime_or_None_if_NULL,
    constants.FIELD_TYPE_DATE: date_or_None_if_NULL,
    constants.FIELD_TYPE_NEWDATE: date_or_None_if_NULL,
    constants.FIELD_TYPE_TIME: timedelta_or_None_if_NULL,

    constants.FIELD_TYPE_BIT: bit_or_None_if_NULL,
    constants.FIELD_TYPE_SET: set_or_None_if_NULL
}

# Decoder protocol
# Each decoder is passed a field object.
# The decoder returns a single value:
# * A callable that given an SQL value, returns a Python object.
# This can be as simple as int or str, etc. If the decoder
# returns None, this decoder will be ignored and the next decoder
# on the stack will be checked.


def default_decoder(*_):
    return bytes_or_None_if_NULL


def default_encoder(_, value):
    """Convert an Instance to a string representation.  If the __str__()
    method produces acceptable output, then you don't need to add the
    class to conversions; it will be handled by the default
    converter. If the exact class is not found in conv, it will use the
    first class it can find for which obj is an instance.
    """
    try:
        conv = next(key for key in simple_type_encoders.keys() if isinstance(value, key))
    except StopIteration:
        return any_to_sql

    return simple_type_encoders.setdefault(type(value), conv)


def simple_decoder(_, field):
    return simple_field_decoders.get(field.type, None)


def simple_encoder(_, value):
    return simple_type_encoders.get(type(value), None)


character_types = {
    constants.FIELD_TYPE_BLOB,
    constants.FIELD_TYPE_STRING,
    constants.FIELD_TYPE_VAR_STRING,
    constants.FIELD_TYPE_VARCHAR,
}


def character_decoder(connection, field):
    if field.type not in character_types:
        return None

    if field.flags & constants.FLAG_BINARY:
        return bytes_or_None_if_NULL

    if field.flags & constants.FLAG_SET:
        return set_or_None_if_NULL

    charset = connection.charset

    def char_to_str(s):
        return s if s is None else s.decode(charset)

    return char_to_str


default_decoders = [
    character_decoder,
    simple_decoder,
    default_decoder,
]

default_encoders = [
    simple_encoder,
    default_encoder,
]


def get_codec(connection, field, codecs):
    for c in codecs:
        func = c(connection, field)
        if func:
            return func
    raise connection.NotSupportedError(("could not encode as SQL", field))


def iter_row_decoder(decoders, row):
    if row is None:
        return None
    return (d(col) for d, col in zip(decoders, row))


def tuple_row_decoder(decoders, row):
    if row is None:
        return None
    return tuple(iter_row_decoder(decoders, row))


default_row_formatter = tuple_row_decoder
