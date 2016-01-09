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

from .cluster import *
from .pool import *
from .upstream import *
from .functional import *

from wsql.connections import UNSET
from wsql.converters import object_row_decoder


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
    connection_args.setdefault('row_formatter', object_row_decoder)

    retries = int(connection_args.pop('retries', 5))
    delay = float(connection_args.pop('delay', 0.2))
    timeout = int(connection_args.pop('timeout', 1))
    master = connection_args.pop("master", "")
    slave = connection_args.pop("slave", "")

    make_connection = uri_parser(lambda servers: retryable(ConnectionPool(Upstream(servers, connect_timeout=timeout, loop=loop, logger=logger, **connection_args),
                                                                          timeout=timeout, loop=loop),
                                                           count=retries, delay=delay))

    master = make_connection(master)
    slave = make_connection(slave)

    if slave:
        return Cluster(master=master, slave=slave)

    return master
