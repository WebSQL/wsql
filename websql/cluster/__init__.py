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


def connect(connection_args, loop=UNSET, logger=None):
    """
    connection_args format:
    uri=[dbcs?:://]hostname[:port]#count"
    master=uri[,uri...];[slave=uri[,uri...];]database=database;[username=username];[password=password];
    :param connection_args: the connection args
    :param loop: if specified the asynchronous version will be returned
    :param logger: the application logger
    """

    def make_connection(uri):
        upstream = Upstream((x.groupdict() for x in host_pattern.finditer(uri)),
                            connect_timeout=timeout, loop=loop, logger=logger, **connection_args)

        return retryable(ConnectionPool(upstream, timeout=timeout, loop=loop), count=retries, delay=delay)

    import re
    host_pattern = re.compile(r"(?:(?:(?:(?:(?P<scheme>dbcs?)://)?(?P<host>[\w\d\.]+)(?::(?P<port>\d+))?(?:#(?P<count>\d+))?),?)+?)")

    if isinstance(connection_args, str):
        connection_args = dict(x.split("=") for x in connection_args.split(";") if x)

    retries = int(connection_args.pop('retries', 5))
    delay = int(connection_args.pop('delay', 0.2))
    timeout = int(connection_args.pop('timeout', 1))
    master = connection_args.pop("master", "")
    slave = connection_args.pop("slave", "")

    if slave:
        slave = make_connection(slave)
        return Cluster(master=master and make_connection(master), slave=slave)

    return make_connection(master)
