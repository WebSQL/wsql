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

    def norm(x):
        if isinstance(x, str):
            x = x.encode('utf-8')
        return x.strip()

    sep = b';'
    statement = b''

    for line in filter(not_comment, map(norm, stream)):
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


def __update_kwargs(kwargs, host, port):
    kw = kwargs.copy()
    if host:
        kw["host"] = host
    if port:
        kw["port"] = port
    return kw


def __deploy_to_master(connection_args, statements, scheme_options):
    """
    deploy scheme to master node
    :param connection_args: the connection arguments
    :param statements: the statements to apply
    """
    database = connection_args.pop('database')
    connection = connect(**connection_args)
    database_bin = database.encode('ascii')

    with connection:
        with connection.cursor() as cursor:
            cursor.execute(b"CREATE SCHEMA IF NOT EXISTS `" + database_bin + b"`" + scheme_options + b";")
            cursor.execute(b"ALTER DATABASE `" + database_bin + b"`" + scheme_options + b";")
            connection.select_db(database)

            for statement in statements:
                try:
                    cursor.execute(statement)
                except:
                    import sys
                    print(statement, file=sys.stderr)
                    raise


def __drop_from_master(connection_args):
    """
    drop scheme from master node
    """
    database = connection_args.pop('database')
    connection = connect(**connection_args)
    database_bin = database.encode('ascii')

    with connection:
        with connection.cursor() as cursor:
            cursor.execute(b"DROP SCHEMA IF EXISTS `" + database_bin + b"`;")


def __deploy_to_all(servers, connection_args, statements, scheme_options):
    """
    apply for all servers asynchronously
    :param servers: the servers in cluster
    :param connection_args: the connection arguments
    :param statements: the statements to apply
    """
    import asyncio
    from asyncio.queues import Queue

    loop = asyncio.new_event_loop()
    database = connection_args.pop('database')
    database_bin = database.encode('ascii')

    @asyncio.coroutine
    def apply(queue, host, port):
        connection = yield from connect(loop=loop, **__update_kwargs(connection_args, host, port))
        try:
            cursor = connection.cursor()
            try:
                yield from cursor.execute(b"CREATE SCHEMA IF NOT EXISTS `" + database_bin + b"`" + scheme_options + b";")
                yield from cursor.execute(b"ALTER DATABASE `" + database_bin + b"`" + scheme_options + b";")
                yield from connection.select_db(database)

                while True:
                    s = yield from queue.get()
                    if s is None:
                        break
                    try:
                        yield from cursor.execute(s)
                    except:
                        import sys
                        print(s, file=sys.stderr)
                        raise
            finally:
                yield from cursor.close()
            yield from connection.commit()
        except:
            yield from connection.rollback()
            raise
        finally:
            connection.close()

    @asyncio.coroutine
    def main_loop():
        for s in statements:
            for q in queues:
                yield from q.put(s)

        for q in queues:
                yield from q.put(None)

    try:
        tasks = []
        queues = []
        for server in servers:
            queues.append(Queue(maxsize=100, loop=loop))
            tasks.append(asyncio.async(apply(queues[-1], server[0], server[1]), loop=loop))

        tasks.append(asyncio.async(main_loop(), loop=loop))
        loop.run_until_complete(asyncio.wait(tasks, loop=loop))
        tasks.clear()
        queues.clear()
    finally:
        loop.close()


def __drop_from_all(servers, connection_args):
    """
    drop from all nodes
    :param servers: the servers in cluster
    :param connection_args: the connection arguments
    """
    import asyncio

    loop = asyncio.new_event_loop()
    database = connection_args.pop('database')
    database_bin = database.encode('ascii')

    @asyncio.coroutine
    def apply(host, port):
        connection = yield from connect(loop=loop, **__update_kwargs(connection_args, host, port))
        try:
            cursor = connection.cursor()
            try:
                yield from cursor.execute(b"DROP SCHEMA IF EXISTS `" + database_bin + b"`;")
            finally:
                yield from cursor.close()
        finally:
            connection.close()

    try:
        tasks = []
        for server in servers:
            tasks.append(asyncio.async(apply(server[0], server[1]), loop=loop))

        loop.run_until_complete(asyncio.wait(tasks, loop=loop))
        tasks.clear()
    finally:
        loop.close()


def deploy_scheme(connection_args, source, for_all=False, scheme_options=b'', **kwargs):
    """
    deploy scheme to cluster
    :param connection_args: the connection arguments, included all nodes and database name
    :param source: the source file(should be open in binary mode)
    :param for_all: apply the statement for all nodes in cluster
    :param kwargs: additional connection arguments
    """

    connection_args = parse_connection_string(connection_args, kwargs)

    masters = set()
    slaves = set()

    def add_server(target, servers):
        for server in servers:
            target.add((server.get('host'), int(server.get('port', 0) or 0)))

    uri_parser(lambda x: add_server(masters, x))(connection_args.pop('master', None))
    uri_parser(lambda x: add_server(slaves, x))(connection_args.pop('slave', None))

    if for_all:
        __deploy_to_all(masters | slaves, connection_args, _statement_iter(source), scheme_options)
    else:
        master = next(iter(masters))
        __deploy_to_master(__update_kwargs(connection_args, master[0], master[1]), _statement_iter(source), scheme_options)


def drop_scheme(connection_args, for_all=False, **kwargs):
    """
    drop the scheme from cluster
    :param connection_args: the
    :param for_all:
    :param kwargs: additional connection arguments
    :return:
    """
    connection_args = parse_connection_string(connection_args, kwargs)

    masters = set()
    slaves = set()

    def add_server(target, servers):
        for server in servers:
            target.add((server.get('host'), int(server.get('port', 0) or 0)))

    uri_parser(lambda x: add_server(masters, x))(connection_args.pop('master', None))
    uri_parser(lambda x: add_server(slaves, x))(connection_args.pop('slave', None))

    if for_all:
        __drop_from_all(masters | slaves, connection_args)
    else:
        master = next(iter(masters))
        __drop_from_master(__update_kwargs(connection_args, master[0], master[1]))
