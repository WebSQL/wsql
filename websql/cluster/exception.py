"""
WebSQL User Exceptions Handler
--------------------

This module implements user exceptions.
"""

from websql import Error, ProgrammingError


__all__ = ["UserError", "handle_error"]


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
