#ifndef _MYSQL_PYTHON__MYSQL_H_
#define _MYSQL_PYTHON__MYSQL_H_

#include "compat.h"

#ifdef MS_WIN32
#include <windows.h>
#endif /* MS_WIN32 */

#include <structmember.h>
#include <mysql.h>
#include <my_config.h>
#include <mysqld_error.h>
#include <errmsg.h>
#include "trace.h"

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

typedef struct {
	PyObject_HEAD
	MYSQL connection;
	int open;
	int autocommit;
} _mysql_connection_object;

extern PyTypeObject _mysql_connection_object_t;

typedef struct {
	PyObject_HEAD
	PyObject *connection;
	MYSQL_RES *result;
	int num_fields;
	int use;
	PyObject *fields;
} _mysql_result_object;

extern PyTypeObject _mysql_result_object_t;

typedef struct {
	PyObject_HEAD
	MYSQL_FIELD field;
	unsigned int index;
} _mysql_field_object;

extern PyTypeObject _mysql_field_object_t;

#define CHECK_CONNECTION(c, res) if (!(c->open)) { _mysql_exception(c); return res;}
#define RESULT_CONNECTION(r) ((_mysql_connection_object *)r->connection)
#define CHECK_RESULT(r, res) CHECK_CONNECTION(RESULT_CONNECTION(r), res)


extern int _mysql_server_init_done;
#if MYSQL_VERSION_ID >= 40000
#define CHECK_SERVER(x) if (!_mysql_server_init_done) { if (mysql_server_init(0, NULL, NULL)) { _mysql_exception(NULL); return x; } else { _mysql_server_init_done = 1;} }
#else
#define CHECK_SERVER(x) if (!_mysql_server_init_done) _mysql_server_init_done = 1
#endif

extern PyObject *_mysql_base_error;
extern PyObject *_mysql_warning;
extern PyObject *_mysql_error;
extern PyObject *_mysql_database_error;
extern PyObject *_mysql_interface_error;
extern PyObject *_mysql_data_error;
extern PyObject *_mysql_operational_error;
extern PyObject *_mysql_integrity_error;
extern PyObject *_mysql_internal_error;
extern PyObject *_mysql_programming_error;
extern PyObject *_mysql_not_supported_error;
extern PyObject *_mysql_error_map;

extern PyObject *
_mysql_exception(_mysql_connection_object *c);

extern char _mysql_connect__doc__[];

extern PyObject *
_mysql_connect(
	PyObject *self,
	PyObject *args,
	PyObject *kwargs);

extern int
_mysql_result_object__init__(
	_mysql_result_object *self,
	PyObject *args,
	PyObject *kwargs);

extern int
_mysql_field_object__init__(
	_mysql_field_object *self,
	PyObject *args,
	PyObject *kwargs);


extern int
_mysql_exceptions_add(
    PyObject* module
);


extern int
_mysql_constants_add(
    PyObject* module
);

#endif
