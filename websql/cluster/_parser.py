# @copyright (c) 2002-2015 Acronis International GmbH. All rights reserved.
# since    $Id: $

__author__ = "Bulat Gaifullin (bulat.gaifullin@acronis.com)"


def parse_connection_string(connection_string):
    """
    parse the connection string
    :param connection_string: the connection string, pairs of key=value  delimited by ;
    :return: the key,value pairs from connection string
    """
    if isinstance(connection_string, str):
        return dict(x.split("=") for x in connection_string.split(";") if x)
    elif isinstance(connection_string, dict):
        return connection_string
    raise ValueError("unsupported format of connection string: %s", type(connection_string))


def uri_parser(callback):
    """
    :param callback: the callback to handle parameters
    :return: connection args
    """
    import re
    uri_scheme = re.compile(r"(?:(?:(?:(?:(?P<scheme>dbcs?)://)?(?P<host>[\w\d\.]+)(?::(?P<port>\d+))?(?:#(?P<count>\d+))?),?)+?)")

    def parser(uri):
        if uri:
            return callback(x.groupdict() for x in uri_scheme.finditer(uri))
    return parser
