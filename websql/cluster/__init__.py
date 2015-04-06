"""
WebSQL Connection Cluster
-------------------------

This module implements pool of connections and connection providers
"""

from .cluster import *
from .pool import *
from .upstream import *
from .functional import *

from websql.connections import UNSET


def connect(connection_args, loop=UNSET, logger=None, **kwargs):
    """
    connection_args format:
    uri=[dbcs?:://]hostname[:port]#count"
    master=uri[,uri...];[slave=uri[,uri...];]database=database;[username=username];[password=password];
    :param connection_args: the connection args
    :param loop: if specified the asynchronous version will be returned
    :param logger: the application logger
    :param kwargs: the additional connection args
    """

    from ._parser import parse_connection_string, uri_parser

    connection_args = parse_connection_string(connection_args, kwargs)
    retries = int(connection_args.pop('retries', 5))
    delay = int(connection_args.pop('delay', 0.2))
    timeout = int(connection_args.pop('timeout', 1))

    make_connection = uri_parser(lambda servers: retryable(ConnectionPool(Upstream(servers, connect_timeout=timeout, loop=loop, logger=logger, **connection_args),
                                                                          timeout=timeout, loop=loop),
                                                           count=retries, delay=delay))

    master = make_connection(connection_args.pop("master", ""))
    slave = make_connection(connection_args.pop("slave", ""))

    if slave:
        return Cluster(master=master, slave=slave)

    return master
