"""
@file
@brief    
@details  Copyright (c) 2001-2014 Acronis
@author   Bulat Gaifullin (bulat.gaifullin@acronis.com)
@since    $Id: $
"""

from MySQLdb import connect
import time


connection = connect(host='testdb.msp.ru.corp.acronis.com', user='mysqluser', password='qwe123QWE', database='rain_test')
cursor = connection.cursor()
timeit = 0
for _ in range(100):
    t = time.perf_counter()
    cursor.execute('select * from groups')
    cursor.fetchall()
    cursor.callproc("groups_get", (1, 1))
    cursor.fetchall()
    timeit += ((time.perf_counter() - t) * 1000)
    print(timeit)

print('%f ms' % (timeit / 100))

del cursor
del connection

print('done')
