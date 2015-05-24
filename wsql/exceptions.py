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


__all__ = ["Warning", "Error", "InterfaceError", "DatabaseError", "DataError", "OperationalError",
           "IntegrityError", "InternalError", "ProgrammingError", "NotSupportedError", "StandardError",
           "UserError", "handle_error"]


Warning = _wsql.exceptions.Warning
Error = _wsql.exceptions.Error
InterfaceError = _wsql.exceptions.InterfaceError
DatabaseError = _wsql.exceptions.DatabaseError
DataError = _wsql.exceptions.DataError
OperationalError = _wsql.exceptions.OperationalError
IntegrityError = _wsql.exceptions.IntegrityError
InternalError = _wsql.exceptions.InternalError
ProgrammingError = _wsql.exceptions.ProgrammingError
NotSupportedError = _wsql.exceptions.NotSupportedError
StandardError = _wsql.exceptions.StandardError


class UserError(ProgrammingError):
    """base class for all user defined exceptions"""
    def __init__(self, code, message):
        super().__init__((code, message))
        self.code = code
        self.message = message


def handle_error(exceptions, error, sep=';'):
    """
        handle exceptions from SQL procedures according to format <classname><sep><message>
        :param exceptions: the module with all user defined exceptions
        :param error: sql error
        :param sep: (optional), the class, message separator
    """
    if isinstance(error, Error):
        try:
            cls, _, msg = error.message.partition(sep)
            exc = getattr(exceptions, cls.strip())
            raise exc(error.code, msg.strip()) from None
        except AttributeError:
            pass
    raise error
