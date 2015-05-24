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


#ifndef __WSQL_RESULT_H__
#define __WSQL_RESULT_H__

#include "connection.h"

#define RESULT_CONNECTION(r) ((wsql_connection *)r->connection)
#define CHECK_RESULT(r, res) CHECK_CONNECTION(RESULT_CONNECTION(r), res)

extern PyTypeObject wsql_result_t;

typedef struct {
    PyObject_HEAD
    PyObject *connection;
    MYSQL_RES *result;
    int num_fields;
    int use;
    int more_rows;
    PyObject *fields;
} wsql_result;

extern int wsql_result__init__(wsql_result *self, PyObject *args, PyObject *kwargs);

#endif //__WSQL_RESULT_H__
