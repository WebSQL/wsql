"""
WebSQL User Exceptions Handler
--------------------

This module implements user exceptions.
"""

import _websql


__all__ = ["Warning", "Error", "InterfaceError", "DatabaseError", "DataError", "OperationalError",
           "IntegrityError", "InternalError", "ProgrammingError", "NotSupportedError", "StandardError",
           "UserError", "handle_error"]


Warning = _websql.exceptions.Warning
Error = _websql.exceptions.Error
InterfaceError = _websql.exceptions.InterfaceError
DatabaseError = _websql.exceptions.DatabaseError
DataError = _websql.exceptions.DataError
OperationalError = _websql.exceptions.OperationalError
IntegrityError = _websql.exceptions.IntegrityError
InternalError = _websql.exceptions.InternalError
ProgrammingError = _websql.exceptions.ProgrammingError
NotSupportedError = _websql.exceptions.NotSupportedError
StandardError = _websql.exceptions.StandardError


class UserError(ProgrammingError):
    """base class for all user defined exceptions"""
    def __init__(self, code, message):
        super().__init__((code, message))
        self.code = code
        self.message = message


def handle_error(exceptions, error, sep=';'):
    if isinstance(error, Error):
        try:
            cls, _, msg = error.message.partition(sep)
            exc = getattr(exceptions, cls.strip())
            raise exc(error.code, msg.strip()) from None
        except AttributeError:
            pass
    raise error
