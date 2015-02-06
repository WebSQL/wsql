"""
WebSQL Connection Fabrics
-------------------------

This module implements pool of connections and connection providers
"""

from .cluster import *
from .pool import *
from .upstream import *
from .functional import *
from .exception import *


def smart_connect(connection_args, nonblocking=True, loop=None, logger=None):
    """
    uri=[dbcs?:://]hostname[:port]#count"
    master=uri[,uri...];[slave=uri[,uri...];]database=database;[username=username];[password=password];
    """

    import re
    host_pattern = re.compile(r"(?:(?:(?:(?:(?P<scheme>dbcs?)://)?(?:(?P<host>[\w\d\.]+))(?::(?P<port>\d+))?(?:#(?P<count>\d+))?),?)+?)")

    if isinstance(connection_args, str):
        connection_args = dict(x.split("=") for x in connection_args.split(";") if x)

    retries = int(connection_args.pop('retries', 5))
    delay = int(connection_args.pop('delay', 0.2))
    timeout = int(connection_args.pop('timeout', 1))

    def make_pool(uri):
        if uri is None:
            return uri

        upstream = Upstream((x.groupdict() for x in host_pattern.finditer(uri)),
                            connect_timeout=timeout, loop=loop, nonblocking=nonblocking, logger=logger)

        return retryable(ConnectionPool(upstream, timeout=timeout, loop=loop, nonblocking=nonblocking), count=retries, delay=delay)

    master = make_pool(connection_args.get('master'))
    slave = make_pool(connection_args.get('slave'))
    if slave:
        return Cluster(master=master, slave=slave)
    elif not master:
        return make_pool("")
    return master
