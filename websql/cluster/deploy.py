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


def __update_kwargs(kwargs, host, port):
    kw = kwargs.copy()
    if host:
        kw["host"] = host
    if port:
        kw["port"] = port
    return kw


def __apply_to_master(connection_args, statements):
    """
    apply statement for one node
    :param connection_args: the connection arguments
    :param statements: the statements to apply
    """
    database = connection_args.pop('database')
    connection = connect(**connection_args)
    database_bin = database.encode('ascii')

    with connection:
        with connection.cursor() as cursor:
            cursor.execute(b"CREATE SCHEMA IF NOT EXISTS `" + database_bin + b"` DEFAULT CHARSET utf8 COLLATE utf8_unicode_ci;")
            cursor.execute(b"ALTER DATABASE `" + database_bin + b"` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;")
            connection.select_db(database)

            for statement in statements:
                cursor.execute(statement)


def __apply_to_all(servers, connection_args, statements):
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
                yield from cursor.execute(b"CREATE SCHEMA IF NOT EXISTS `" + database_bin + b"` DEFAULT CHARSET utf8 COLLATE utf8_unicode_ci;")
                yield from cursor.execute(b"ALTER DATABASE `" + database_bin + b"` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;")
                yield from cursor.nextset()
                yield from connection.select_db(database)

                while True:
                    s = yield from queue.get()
                    if s is None:
                        break
                    yield from cursor.execute(s)
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


def apply_statements(connection_args, source, for_all=False):
    """
    :param connection_args: the connection arguments, included all nodes and database name
    :param source: the source file(should be open in binary mode)
    :param for_all: apply the statement for all nodes in cluster
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
        __apply_to_all(masters | slaves, connection_args, _statement_iter(source))
    else:
        master = next(iter(masters))
        __apply_to_master(__update_kwargs(connection_args, master[0], master[1]), _statement_iter(source))
