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

try:
    from _capabilities import CapabilityTestCases
    import _wsql_context
except ImportError:  # pragma: no cover
    from ._capabilities import CapabilityTestCases
    from . import _wsql_context


class TestWebSQLCapabilities(CapabilityTestCases):
    @classmethod
    def get_context(cls):
        return _wsql_context.Context(_wsql_context.Configuration())


class TesWebSQLAsyncWebSQLAsyncCapabilities(CapabilityTestCases):
    @classmethod
    def get_context(cls):
        return _wsql_context.Context(_wsql_context.ConfigurationAsync())

del CapabilityTestCases
