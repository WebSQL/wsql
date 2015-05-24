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

#ifndef __WSQL_COMMON_H__
#define __WSQL_COMMON_H__

#include "compat.h"
#include "trace.h"

#ifdef MS_WIN32
#include <windows.h>
#endif /* MS_WIN32 */

#include <structmember.h>
#include <mysql.h>
#include <my_config.h>
#include <mysqld_error.h>
#include <errmsg.h>

#define JOIN(X,Y) DO_JOIN1(X,Y)
#define DO_JOIN1(X,Y) DO_JOIN2(X,Y)
#define DO_JOIN2(X,Y) X##Y

#define DO_STRINGIFY(x) #x
#define STRINGIFY(x) DO_STRINGIFY(x)

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

#ifdef HAVE_MYSQL_NONBLOCKING_CLIENT
#define HAVE_ASYNCIO
#endif

extern PyObject *wsql_standard_error;
extern PyObject *wsql_warning;
extern PyObject *wsql_error;
extern PyObject *wsql_database_error;
extern PyObject *wsql_interface_error;
extern PyObject *wsql_data_error;
extern PyObject *wsql_operational_error;
extern PyObject *wsql_integrity_error;
extern PyObject *wsql_internal_error;
extern PyObject *wsql_programming_error;
extern PyObject *wsql_not_supported_error;
extern PyObject *wsql_error_map;

#endif //__WSQL_COMMON_H__
