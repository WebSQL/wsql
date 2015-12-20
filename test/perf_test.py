# encoding: utf-8

import sys
import asyncio
from wsql import cluster


io_loop = asyncio.new_event_loop()


CONN_PARAMS = {
    'master': '127.0.0.1#30',
    'user': 'root',
    'password': '',
    'database': 'test',
    'defer_warnings': True
}


POOL = None


@asyncio.coroutine
def prepare():
    global POOL
    POOL = cluster.connect(CONN_PARAMS,
                           loop=io_loop,
                           retries=1,
                           delay=0,
                           timeout=100)


def _q(sql, *args):
    @asyncio.coroutine
    def query(connection):
        cursor = connection.cursor()
        try:
            yield from cursor.execute(sql, args)
            return (yield from cursor.fetchall())
        finally:
            yield from cursor.close()

    return query


def test_db_sleep():
    return POOL.execute(_q(b"SELECT SLEEP(%s)", 1))


@asyncio.coroutine
def test_table():
    @asyncio.coroutine
    def query(connection):
        q = (b'CREATE TABLE IF NOT EXISTS `test_table` ( '
             b'`id` int NOT NULL STORAGE MEMORY AUTO_INCREMENT, '
             b'`key` varchar(20) NOT NULL STORAGE MEMORY, '
             b'`value` varchar(20) NOT NULL STORAGE MEMORY, '
             b'`int` bigint NOT NULL STORAGE MEMORY, '
             b'`text` text CHARACTER SET utf8 COLLATE utf8_bin NOT NULL STORAGE MEMORY, '
             b'`ts` datetime STORAGE MEMORY DEFAULT CURRENT_TIMESTAMP, '
             b'PRIMARY KEY (`id`) '
             b') ENGINE=`InnoDB` COMMENT="";')

        cursor = connection.cursor()
        try:
            yield from cursor.execute(q)

            with open('data.sql', 'rb') as f:
                for x, q1 in enumerate(f):
                    yield from cursor.execute(q1)
                    sys.stdout.write('\r%04d ' % x)
        finally:
            yield from cursor.close()

    yield from POOL.execute(query)



@asyncio.coroutine
def test_table_select():
    data = yield from POOL.execute(_q(b'select * from test_table limit 1000000'))
    print(len(data))


@asyncio.coroutine
def test_table_select_one_by_one():
    @asyncio.coroutine
    def query(connection):
        cursor = connection.cursor()
        try:
            yield from cursor.execute(b'select * from test_table limit 1000000')
            c = 0
            data = (yield from cursor.fetchone())
            while data:
                data = yield from cursor.fetchone()
                c += 1
            print(c)
        finally:
            yield from cursor.close()

    yield from POOL.execute(query)


@asyncio.coroutine
def test():
    tasks = []
    add_task = tasks.append
    for _ in range(1800):
        add_task(asyncio.async(test_db_sleep(), loop=io_loop))

    yield from asyncio.wait(tasks, loop=io_loop)


def periodic_callback():
    sys.stdout.flush()
    if not io_loop.is_closed():
        io_loop.call_later(1, periodic_callback)


if __name__ == '__main__':
    periodic_callback()
    t = io_loop.time()
    print(io_loop.run_until_complete(prepare()))
    print("Preparing time: {}".format(io_loop.time() - t))

    t = io_loop.time()
    print("Starting test: Table creation")
    print(io_loop.run_until_complete(test_table()))
    print("Execution time: {}".format(io_loop.time() - t))

    t = io_loop.time()
    print("Starting test: Table select one by one")
    print(io_loop.run_until_complete(test_table_select_one_by_one()))
    print("Execution time: {}".format(io_loop.time() - t))

    t = io_loop.time()
    print("Starting test: Table select")
    print(io_loop.run_until_complete(test_table_select()))
    print("Execution time: {}".format(io_loop.time() - t))

    t = io_loop.time()
    print("Starting test: Sleep concurrency")
    print(io_loop.run_until_complete(test()))
    print("Execution time: {}".format(io_loop.time() - t))

    @asyncio.coroutine
    def cleanup():
        def query(connection):
            cursor = connection.cursor()
            try:
                yield from cursor.execute(b"DROP TABLE `test_table`")
            finally:
                yield from cursor.close()

        yield from POOL.execute(query)

    print(io_loop.run_until_complete(cleanup()))
    print('stop')
    io_loop.stop()
    io_loop.close()
