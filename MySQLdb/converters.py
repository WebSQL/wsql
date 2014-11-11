"""
MySQLdb type conversion module
------------------------------



"""

try:
    from ._mysql import constants
    from .times import datetime_to_sql, timedelta_to_sql, \
        timedelta_or_orig, datetime_or_orig, date_or_orig, \
        timestamp_or_orig
except SystemError:
    from _mysql import constants
    from times import datetime_to_sql, timedelta_to_sql, \
        timedelta_or_orig, datetime_or_orig, date_or_orig, \
        timestamp_or_orig

import datetime
from decimal import Decimal

NULL = constants.NULL


def bool_to_sql(_, value):
    """Convert a Python bool to an SQL literal."""
    return str(int(value))


def sql_to_set(value):
    """Convert MySQL SET column to Python set."""
    return {i for i in value.split(',') if i}


def set_to_sql(connection, value):
    """Convert a Python set to an SQL literal."""
    return connection.string_literal(','.join(value)).decode(connection.charset)


def object_to_sql(connection, obj):
    """Convert something into a string via str().
    The result will not be quoted."""
    return connection.escape_string(str(obj)).decode(connection.charset)


def unicode_to_sql(connection, value):
    """Convert a unicode object to a string using the connection encoding."""
    charset = connection.charset
    return connection.string_literal(value.encode(charset)).decode(charset)


def float_to_sql(_, value):
    return '%.15g' % value


def none_to_sql(*_):
    """Convert None to NULL."""
    return NULL


def none_if_null(func):
    if func is None:
        return func

    def _none_if_null(value):
        return value if value is None else func(value)
    _none_if_null.__name__ = func.__name__ + "_or_None_if_NULL"
    return _none_if_null


int_or_None_if_NULL = none_if_null(int)
float_or_None_if_NULL = none_if_null(float)
Decimal_or_None_if_NULL = none_if_null(Decimal)
SET_to_Set_or_None_if_NULL = none_if_null(sql_to_set)
timestamp_or_None_if_NULL = none_if_null(timestamp_or_orig)
datetime_or_None_if_NULL = none_if_null(datetime_or_orig)
date_or_None_if_NULL = none_if_null(date_or_orig)
timedelta_or_None_if_NULL = none_if_null(timedelta_or_orig)


def object_to_quoted_sql(connection, obj):
    """Convert something into a SQL string literal."""
    if isinstance(obj, str):
        return unicode_to_sql(connection, obj)
    return connection.string_literal(str(obj))


def instance_to_sql(connection, obj, conv):
    """Convert an Instance to a string representation.  If the __str__()
    method produces acceptable output, then you don't need to add the
    class to conversions; it will be handled by the default
    converter. If the exact class is not found in conv, it will use the
    first class it can find for which obj is an instance.
    """

    if type(obj) in conv:
        return conv[type(obj)](obj, conv)

    try:
        klass = next(key for key in conv.keys() if isinstance(obj, key))
    except StopIteration:
        return conv[str](obj, conv)

    return conv.setdefault(type(obj), klass)(connection, obj)


def array_to_sql(connection, obj):
    return connection.string_literal(obj.tostring())


simple_type_encoders = {
    int: object_to_sql,
    float: float_to_sql,
    type(None): none_to_sql,
    object: instance_to_sql,
    bool: bool_to_sql,
    datetime.datetime: datetime_to_sql,
    datetime.timedelta: timedelta_to_sql,
    set: set_to_sql,
    str: object_to_quoted_sql
}

# This is for MySQL column types that can be converted directly
# into Python types without having to look at metadata (flags,
# character sets, etc.). This should always be used as the last
# resort.
simple_field_decoders = {
    constants.FIELD_TYPE_TINY: int_or_None_if_NULL,
    constants.FIELD_TYPE_SHORT: int_or_None_if_NULL,
    constants.FIELD_TYPE_LONG: int_or_None_if_NULL,
    constants.FIELD_TYPE_FLOAT: float_or_None_if_NULL,
    constants.FIELD_TYPE_DOUBLE: float_or_None_if_NULL,
    constants.FIELD_TYPE_DECIMAL: Decimal_or_None_if_NULL,
    constants.FIELD_TYPE_NEWDECIMAL: Decimal_or_None_if_NULL,
    constants.FIELD_TYPE_LONGLONG: int_or_None_if_NULL,
    constants.FIELD_TYPE_INT24: int_or_None_if_NULL,
    constants.FIELD_TYPE_YEAR: int_or_None_if_NULL,
    constants.FIELD_TYPE_SET: SET_to_Set_or_None_if_NULL,
    constants.FIELD_TYPE_TIMESTAMP: timestamp_or_None_if_NULL,
    constants.FIELD_TYPE_DATETIME: datetime_or_None_if_NULL,
    constants.FIELD_TYPE_TIME: timedelta_or_None_if_NULL,
    constants.FIELD_TYPE_DATE: date_or_None_if_NULL,
}

# Decoder protocol
# Each decoder is passed a field object.
# The decoder returns a single value:
# * A callable that given an SQL value, returns a Python object.
# This can be as simple as int or str, etc. If the decoder
# returns None, this decoder will be ignored and the next decoder
# on the stack will be checked.


def default_decoder(*_):
    return str


def default_encoder(_):
    return object_to_quoted_sql


def simple_decoder(_, field):
    return simple_field_decoders.get(field.type, None)


def simple_encoder(value):
    return simple_type_encoders.get(type(value), None)

character_types = [
    constants.FIELD_TYPE_BLOB,
    constants.FIELD_TYPE_STRING,
    constants.FIELD_TYPE_VAR_STRING,
    constants.FIELD_TYPE_VARCHAR,
]


def character_decoder(connection, field):
    if field.type not in character_types:
        return None

    charset = connection.charset

    def char_to_unicode(s):
        return s if s is None else s.encode(charset)

    if field.charsetnr == 63:  # BINARY
        return char_to_unicode

    return str

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
    # the default codec is guaranteed to work


def iter_row_decoder(decoders, row):
    if row is None:
        return None
    return (d(col) for d, col in zip(decoders, row))


def tuple_row_decoder(decoders, row):
    if row is None:
        return None
    return tuple(iter_row_decoder(decoders, row))


default_row_formatter = tuple_row_decoder
