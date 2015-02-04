# @copyright (c) 2002-2015 Acronis International GmbH. All rights reserved.
# since    $Id: $

__author__ = "Bulat Gaifullin (bulat.gaifullin@acronis.com)"

from .functional import TransactionScope

__all__ = ["cluster"]


class _Cluster:
    """asynchronous commutator"""

    def __init__(self, read_connection, write_connection):
        """
        Construction
        :param read_connection:  the connection to perform read requests
        :param write_connection: the connection to perform write requests
        """
        self.__cluster = [read_connection, write_connection]

    def execute(self, request):
        """execute handler"""

        return self.__cluster[isinstance(request, TransactionScope)].execute(request)


cluster = _Cluster
