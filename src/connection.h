/*
WSQL
====
An asynchronous python interface to MySQL
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
*/


#ifndef __WSQL_CONNECTION_H__
#define __WSQL_CONNECTION_H__

#include "common.h"

#define CHECK_CONNECTION(c, res) if (!(c->open)) { wsql_raise_error(c); return res;}


extern PyTypeObject wsql_connection_t;

typedef struct {
    PyObject_HEAD
    MYSQL connection;
    int open;
    int autocommit;
    int connected;
} wsql_connection;

extern const char wsql_connect__doc__[];

extern PyObject* wsql_connect(PyObject *self, PyObject *args, PyObject *kwargs);

extern PyObject* wsql_raise_error(wsql_connection *c);

extern int wsql_server_init_done;

#if MYSQL_VERSION_ID >= 40000
#define CHECK_SERVER(x) \
if (!wsql_server_init_done) {\
    if (mysql_server_init(0, NULL, NULL)) {\
        wsql_raise_error(NULL); return x;\
    }\
    else {\
        wsql_server_init_done = 1;\
    }\
}
#else
#define CHECK_SERVER(x) if (!wsql_server_init_done) wsql_server_init_done = 1
#endif

#endif //__WSQL_CONNECTION_H__