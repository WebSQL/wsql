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

#include "connection.h"
#include "module.h"

PyObject *wsql_standard_error;
PyObject *wsql_warning;
PyObject *wsql_error;
PyObject *wsql_database_error;
PyObject *wsql_interface_error;
PyObject *wsql_data_error;
PyObject *wsql_operational_error;
PyObject *wsql_integrity_error;
PyObject *wsql_internal_error;
PyObject *wsql_programming_error;
PyObject *wsql_not_supported_error;
PyObject *wsql_error_map;

bool is_connection_lost(int error_code)
{
    switch (error_code)
    {
    case CR_SERVER_GONE_ERROR:
    case CR_SERVER_LOST:
        return true;
    default:
        return false;
    }
}

bool is_retryable_error(int error_code)
{
    /// connection error
    if (error_code >= CR_SOCKET_CREATE_ERROR && error_code <= CR_SERVER_GONE_ERROR)
        return true;
    /// lost connection
    if (error_code == CR_SERVER_LOST || error_code == ER_LOCK_DEADLOCK)
        return true;

    return false;
}

bool is_deadlock(int error_code)
{
    return error_code == ER_LOCK_DEADLOCK;
}

PyObject* wsql_set_exception(PyObject* cls, int code, const char* message)
{
    PyObject *py_code = PyLong_FromLong(code);
    PyObject *py_message = PyString_FromString(message);
    if (cls == NULL)
    {
        cls = PyDict_GetItem(wsql_error_map, py_code);
        if (!cls)
            cls = code < ER_ERROR_FIRST ? wsql_internal_error : wsql_operational_error;
    }

    PyObject* exc = PyObject_CallFunction(cls, "OO", py_code, py_message);
    if (exc)
    {
        PyObject_SetAttrString(exc, "code", py_code);
        PyObject_SetAttrString(exc, "message", py_message);
        PyErr_SetObject(cls, exc);
    }
    Py_XDECREF(exc);
    Py_XDECREF(py_code);
    Py_XDECREF(py_message);
    return NULL;
}

PyObject* wsql_raise_error(wsql_connection *c)
{
    int error_code;

    if (!wsql_server_init_done || !c)
        return wsql_set_exception(wsql_internal_error, -1, "server not initialized");

    error_code = mysql_errno(&(c->connection));

    TRACE2("%p, %d", c, error_code);
    if (is_connection_lost(error_code))
        c->connected = 0;

    if (!error_code)
        return wsql_set_exception(wsql_interface_error, -1, "unknown error");

    if (error_code > CR_MAX_ERROR)
        return wsql_set_exception(wsql_interface_error, -1, "error totally whack");

    return wsql_set_exception(NULL, error_code, mysql_error(&(c->connection)));
}

PyObject* wsql_check_error_code(PyObject* args, bool (*checker)(int))
{
    int result = 0;
    PyObject* exception;

    if (!PyArg_ParseTuple(args, "O", &exception))
        return NULL;

    if (PyObject_IsInstance(exception, wsql_error))
    {
        PyObject* code = PyObject_GetAttrString(exception, "code");
        if (code && checker(PyLong_AsLong(code)))
            result = 1;

        Py_XDECREF(code);
    }
    return PyBool_FromLong(result);
}

const char wsql_is_retryable_error__doc__[] =
"Return True if error is retryable, otherwise False\n";

PyObject* wsql_is_retryable_error(PyObject* self, PyObject* args)
{
    return wsql_check_error_code(args, &is_retryable_error);
}

const char wsql_is_deadlock_error__doc__[] =
"Return True if error means dead-lock, otherwise False\n";

PyObject* wsql_is_deadlock_error(PyObject* self, PyObject* args)
{
    return wsql_check_error_code(args, &is_deadlock);
}

const char wsql_exceptions__doc__[] =
"All Exceptions according to DB API 2.0\n";

static PyMethodDef wsql_exceptions_methods[] = {
    {
        "is_retryable",
        (PyCFunction)wsql_is_retryable_error,
        METH_VARARGS,
        wsql_is_retryable_error__doc__
    },
    {
        "is_deadlock",
        (PyCFunction)wsql_is_deadlock_error,
        METH_VARARGS,
        wsql_is_deadlock_error__doc__
    },
    {NULL, NULL} /* sentinel */
};

#ifdef PY3K
PyModuleDef wsql_exceptions_module =
{
    PyModuleDef_HEAD_INIT,
    STRINGIFY(MODULE_NAME) ".exceptions", /* name of module */
    wsql_exceptions__doc__,             /* module documentation, may be NULL */
    -1,                                   /* size of per-interpreter state of the module,
                                             or -1 if the module keeps state in global variables. */
    wsql_exceptions_methods,
    NULL, NULL, NULL, NULL
};
#endif  // PY3K

int
wsql_exceptions_init(PyObject* module)
{
    wsql_standard_error = NULL;
    wsql_warning = NULL;
    wsql_error = NULL;
    wsql_database_error = NULL;
    wsql_interface_error = NULL;
    wsql_data_error = NULL;
    wsql_operational_error = NULL;
    wsql_integrity_error = NULL;
    wsql_internal_error = NULL;
    wsql_programming_error = NULL;
    wsql_not_supported_error = NULL;
    wsql_error_map = NULL;

    PyObject * warning_base = NULL, *exceptions = NULL, *tmp = NULL;
#ifdef PY3K
    exceptions = PyModule_Create(&wsql_exceptions_module);
#else
    exceptions = Py_InitModule3(STRINGIFY(MODULE_NAME) ".exceptions", wsql_exceptions_methods, wsql_exceptions__doc__);
#endif
    if (!exceptions)
        return -1;

    if (! (wsql_standard_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.StandardError",
        "Exception related to operation with MySQL.",
        PyExc_Exception,
        0))) goto on_error;


    if (!(warning_base = Py_BuildValue("(OO)", PyExc_Warning, wsql_standard_error))) goto on_error;

    if (!(wsql_warning = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.Warning",
        "Exception raised for important warnings like data truncations while inserting, etc.",
        warning_base,
        0))) goto on_error;

    Py_DECREF(warning_base);
    warning_base = NULL;

    if (!(wsql_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.Error",
        "Exception that is the base class of all other error exceptions (not Warning).",
        wsql_standard_error,
        0))) goto on_error;

    if (!(wsql_interface_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.InterfaceError",
        "Exception raised for errors that are related to the database interface rather than the database itself.",
        wsql_error,
        0))) goto on_error;

    if (!(wsql_database_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.DatabaseError",
        "Exception raised for errors that are related to the database.",
        wsql_error,
        0))) goto on_error;

    if (!(wsql_data_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.DataError",
        "Exception raised for errors that are due to problems with the \n" \
        "processed data like division by zero, numeric value out of range, etc.",
        wsql_database_error,
        0))) goto on_error;


    if (!(wsql_operational_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.OperationalError",
        "Exception raised for errors that are related to the database's\n" \
        "operation and not necessarily under the control of the programmer," \
        "e.g. an unexpected disconnect occurs, the data source name is not" \
        "found, a transaction could not be processed, a memory allocation" \
        "error occurred during processing, etc.",
        wsql_database_error,
        0))) goto on_error;


    if (!(wsql_integrity_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.IntegrityError",
        "Exception raised when the relational integrity of the database" \
        "is affected, e.g. a foreign key check fails, duplicate key, etc.",
        wsql_database_error,
        0))) goto on_error;


    if (!(wsql_internal_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.InternalError",
        "Exception raised when the database encounters an internal" \
        "error, e.g. the cursor is not valid anymore, the transaction is out of sync, etc.",
        wsql_database_error,
        0))) goto on_error;

    if (!(wsql_programming_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.ProgrammingError",
        "Exception raised for programming errors, e.g. table not found" \
        "or already exists, syntax error in the SQL statement, wrong number" \
        "of parameters specified, etc.",
        wsql_database_error,
        0))) goto on_error;


    if (!(wsql_not_supported_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.NotSupportedError",
        "Exception raised in case a method or database API was used" \
        "which is not supported by the database, e.g. requesting a" \
        ".rollback() on a connection that does not support transaction or" \
        "has transactions turned off.",
        wsql_database_error,
        0))) goto on_error;

    if (PyModule_AddObject(exceptions, "StandardError", wsql_standard_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "Warning", wsql_warning) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "Error", wsql_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "DatabaseError", wsql_database_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "InterfaceError", wsql_interface_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "DataError", wsql_data_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "OperationalError", wsql_operational_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "IntegrityError", wsql_integrity_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "InternalError", wsql_internal_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "ProgrammingError", wsql_programming_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "NotSupportedError", wsql_not_supported_error) < 0) goto on_error;

    if (!(wsql_error_map = PyDict_New())) goto on_error;

#define SET_ERROR_CODE(code, cls) \
    { tmp = PyLong_FromLong(code); PyDict_SetItem(wsql_error_map, tmp, cls); Py_DECREF(tmp); }

    SET_ERROR_CODE(ER_DB_CREATE_EXISTS, wsql_programming_error);
    SET_ERROR_CODE(ER_SYNTAX_ERROR, wsql_programming_error);
    SET_ERROR_CODE(ER_PARSE_ERROR, wsql_programming_error);
    SET_ERROR_CODE(ER_NO_SUCH_TABLE, wsql_programming_error);
    SET_ERROR_CODE(ER_WRONG_DB_NAME, wsql_programming_error);
    SET_ERROR_CODE(ER_WRONG_TABLE_NAME, wsql_programming_error);
    SET_ERROR_CODE(ER_FIELD_SPECIFIED_TWICE, wsql_programming_error);
    SET_ERROR_CODE(ER_INVALID_GROUP_FUNC_USE, wsql_programming_error);
    SET_ERROR_CODE(ER_UNSUPPORTED_EXTENSION, wsql_programming_error);
    SET_ERROR_CODE(ER_TABLE_MUST_HAVE_COLUMNS, wsql_programming_error);
    SET_ERROR_CODE(ER_CANT_DO_THIS_DURING_AN_TRANSACTION, wsql_programming_error);

    SET_ERROR_CODE(ER_WRONG_DB_NAME, wsql_programming_error);
    SET_ERROR_CODE(ER_WRONG_TABLE_NAME, wsql_programming_error);
    SET_ERROR_CODE(ER_FIELD_SPECIFIED_TWICE, wsql_programming_error);
    SET_ERROR_CODE(ER_INVALID_GROUP_FUNC_USE, wsql_programming_error);
    SET_ERROR_CODE(ER_UNSUPPORTED_EXTENSION, wsql_programming_error);
    SET_ERROR_CODE(ER_TABLE_MUST_HAVE_COLUMNS, wsql_programming_error);
    SET_ERROR_CODE(ER_CANT_DO_THIS_DURING_AN_TRANSACTION, wsql_programming_error);

    SET_ERROR_CODE(ER_WARN_DATA_TRUNCATED, wsql_data_error);
    SET_ERROR_CODE(ER_WARN_NULL_TO_NOTNULL, wsql_data_error);
    SET_ERROR_CODE(ER_WARN_DATA_OUT_OF_RANGE, wsql_data_error);
    SET_ERROR_CODE(ER_NO_DEFAULT, wsql_data_error);
    SET_ERROR_CODE(ER_PRIMARY_CANT_HAVE_NULL, wsql_data_error);
    SET_ERROR_CODE(ER_DATA_TOO_LONG, wsql_data_error);
    SET_ERROR_CODE(ER_DATETIME_FUNCTION_OVERFLOW, wsql_data_error);
    SET_ERROR_CODE(ER_PRIMARY_CANT_HAVE_NULL, wsql_data_error);

    SET_ERROR_CODE(ER_DUP_ENTRY, wsql_integrity_error);
    SET_ERROR_CODE(ER_NO_REFERENCED_ROW, wsql_integrity_error);
    SET_ERROR_CODE(ER_NO_REFERENCED_ROW_2, wsql_integrity_error);
    SET_ERROR_CODE(ER_ROW_IS_REFERENCED, wsql_integrity_error);
    SET_ERROR_CODE(ER_ROW_IS_REFERENCED_2, wsql_integrity_error);
    SET_ERROR_CODE(ER_CANNOT_ADD_FOREIGN, wsql_integrity_error);

    SET_ERROR_CODE(ER_WARNING_NOT_COMPLETE_ROLLBACK, wsql_not_supported_error);
    SET_ERROR_CODE(ER_NOT_SUPPORTED_YET, wsql_not_supported_error);
    SET_ERROR_CODE(ER_FEATURE_DISABLED, wsql_not_supported_error);
    SET_ERROR_CODE(ER_UNKNOWN_STORAGE_ENGINE, wsql_not_supported_error);

#undef SET_ERROR_CODE

    if (PyModule_AddObject(module, "exceptions", exceptions) < 0) goto on_error;

    return 0;
    
on_error:
    Py_XDECREF(wsql_standard_error);
    Py_XDECREF(wsql_warning);
    Py_XDECREF(wsql_error);
    Py_XDECREF(wsql_database_error);
    Py_XDECREF(wsql_interface_error);
    Py_XDECREF(wsql_data_error);
    Py_XDECREF(wsql_operational_error);
    Py_XDECREF(wsql_integrity_error);
    Py_XDECREF(wsql_internal_error);
    Py_XDECREF(wsql_programming_error);
    Py_XDECREF(wsql_not_supported_error);
    Py_XDECREF(wsql_error_map);
    Py_XDECREF(warning_base);
    Py_XDECREF(exceptions);
    return -1;
}
