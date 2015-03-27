"""
WebSQL tools to work with scheme
--------------------

This module implements utilities to deploy scheme
"""

from ._parser import parse_connection_string, uri_parser
from websql import connect


def _statement_iter(stream):
    delimiter = b"DELIMITER"

    def not_comment(x):
        return not (x.startswith(b'--') or x.startswith(b'#'))

    sep = b';'
    statement = b''

    for line in filter(not_comment, map(lambda x: x.strip(), stream)):
        if line.startswith(delimiter):
            sep = line[len(delimiter) + 1:].strip()
            statement = b''
        else:
            statement += line + b'\n'
            while True:
                p = statement.find(sep)
                if p != -1:
                    yield statement[0:p]
                    statement = statement[p + len(sep):]
                else:
                    break


def get_connection(loop=None, database=None, **kwargs):
    class Holder:
        connection = None
        cursor = None

    connection = connect(**kwargs)
    cursor = connection.cursor()
    database_bin = database.encode(connection.charset)
    cursor.execute(b"CREATE SCHEMA IF NOT EXISTS `" + database_bin + b"` DEFAULT CHARSET utf8 COLLATE utf8_unicode_ci;")
    cursor.execute(b"ALTER DATABASE `" + database_bin + b"` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;")
    connection.select_db(database)

    holder = Holder()
    holder.connection = connection
    holder.cursor = cursor
    return holder


def __deploy_scheme(connections, statements):
    """
    deploy scheme synchronously
    :param connections: the database connections
    :param statements: the statements
    """
    for statement in statements:
        for c in connections:
            c.cursor.execute(statement)


def __deploy_scheme_parallel(connections, statements):
    """
    deploy scheme to nodes in parallel
    :param connections: the database connections
    :param statements: the statements iterator
    """
    from threading import Thread
    from queue import Queue

    def worker(cursor, queue):
        while True:
            st = queue.get()
            if st is None:
                break
            cursor.execute(st)

    threads = []
    for c in connections:
        c.queue = Queue()
        t = Thread(target=worker, args=(c.cursor, c.queue))
        t.start()
        threads.append(t)

    for s in statements:
        for c in connections:
            c.queue.put(s)

    for c in connections:
        c.queue.put(None)

    for t in threads:
        t.join()


def update_kwargs(kwargs, host, port):
    kw = kwargs.copy()
    if host:
        kw["host"] = host
    if port:
        kw["port"] = port
    return kw


def deploy_scheme(connection_args, source, for_all=False, parallel=False):
    """
    :param connection_args: the connection arguments, included all nodes and database name
    :param source: the source file(should be open in binary mode)
    :param for_all: apply the statement for all nodes in cluster
    :param parallel: run the statements in parallel (by using threads) applicable only if for_all
    """

    connection_args = parse_connection_string(connection_args)

    masters = set()
    slaves = set()

    def add_server(target, servers):
        for server in servers:
            target.add((server.get('host'), int(server.get('port', 0) or 0)))

    uri_parser(lambda x: add_server(masters, x))(connection_args.pop('master', None))
    uri_parser(lambda x: add_server(slaves, x))(connection_args.pop('slave', None))

    if for_all:
        connections = [get_connection(**update_kwargs(connection_args, s[0], s[1])) for s in masters | slaves]
        method = __deploy_scheme_parallel if parallel else __deploy_scheme
    else:
        s = next(iter(masters))
        connections = [get_connection(**update_kwargs(connection_args, s[0], s[1]))]
        method = __deploy_scheme

    method(connections, _statement_iter(source))

    for c in connections:
        c.cursor.close()
        c.connection.commit()
