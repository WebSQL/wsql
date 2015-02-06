# @copyright (c) 2002-2015 Acronis International GmbH. All rights reserved.
# since    $Id: $

__author__ = "Bulat Gaifullin (bulat.gaifullin@acronis.com)"

from .functional import TransactionScope
from websql import NotSupportedError

__all__ = ["Cluster"]


class Cluster:
    """asynchronous commutator"""

    def __init__(self, master, slave):
        """
        Construction
        :param master:  the connection to perform write request
        :param slave: the connection to perform read requests
        """
        self._cluster = [slave, master]

    def execute(self, request):
        """execute handler"""
        connection = self._cluster[isinstance(request, TransactionScope)]
        if not connection:
            raise NotSupportedError((-1, "the operation is not permitted on read-only cluster"))

        return connection.execute(request)
