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

from .functional import TransactionScope
from wsql import NotSupportedError

__all__ = ["Cluster"]


class Cluster:
    """asynchronous commutator"""

    def __init__(self, master, slave):
        """
        Construction
        :param master:  the connection to perform write query
        :param slave: the connection to perform read querys
        """
        self._cluster = [slave, master]

    def execute(self, query):
        """execute handler"""
        connection = self._cluster[isinstance(query, TransactionScope)]
        if not connection:
            raise NotSupportedError((-1, "the operation is not permitted on read-only cluster"))

        return connection.execute(query)
