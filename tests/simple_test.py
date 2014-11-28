from MySQLdb import connect
import asyncio

COUNT = 1


@asyncio.coroutine
def simple_query(_loop, connection, counter=None, timeit=0):
    if connection is None:
        connection = yield from connect(host='testdb.msp.ru.corp.acronis.com',
                                        user='mysqluser', password='qwe123QWE',
                                        database='rain_test',
                                        nonblocking=True, loop=_loop)

    counter = counter or 1
    cursor = connection.cursor()
    try:
        import time
        t = time.perf_counter()
        yield from cursor.execute("select * from groups")
        print((yield from cursor.fetchall()))
        timeit += ((time.perf_counter() - t) * 1000)
        yield from cursor.callproc("groups_get", (1, 1))
        print((yield from cursor.fetchall()))
    except Exception:
        import traceback
        traceback.print_exc()
    del cursor
    if counter < COUNT:
        asyncio.async(simple_query(_loop, connection, counter + 1, timeit))
    else:
        print("average time %f" % (timeit / (COUNT + 1)))
        del connection
        _loop.stop()

loop = asyncio.get_event_loop()

try:
    asyncio.async(simple_query(loop, None))
    loop.run_forever()
finally:
    loop.close()

