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

def parse_connection_string(connection_args, kwargs=None):
    """
    parse the connection string
    :param connection_args: the connection string, pairs of key=value  delimited by ;
    :return: the key,value pairs from connection string
    """
    if isinstance(connection_args, str):
        connection_args = dict(x.split("=") for x in connection_args.split(";") if x)
    elif isinstance(connection_args, dict):
        connection_args = connection_args.copy()
    else:
        raise ValueError("unsupported format of connection string: %s", type(connection_args))

    if kwargs:
        connection_args.update(kwargs)
    return connection_args


def uri_parser(callback):
    """
    :param callback: the callback to handle parameters
    :return: connection args
    """
    import re
    uri_scheme = re.compile(r"(?:(?:(?:(?:(?P<scheme>dbcs?)://)?(?P<host>[\w\d\.]+)(?::(?P<port>\d+))?(?:#(?P<count>\d+))?),?)+?)")

    def parser(uri):
        if uri:
            if isinstance(uri, list):
                import itertools
                iters = (uri_scheme.finditer(u) for u in uri)
                return callback(x.groupdict() for x in itertools.chain(*iters))
            return callback(x.groupdict() for x in uri_scheme.finditer(uri))
    return parser
