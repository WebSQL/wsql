#include "mysqlmod.h"

PyObject *_mysql_standard_error;
PyObject *_mysql_warning;
PyObject *_mysql_error;
PyObject *_mysql_database_error;
PyObject *_mysql_interface_error;
PyObject *_mysql_data_error;
PyObject *_mysql_operational_error;
PyObject *_mysql_integrity_error;
PyObject *_mysql_internal_error;
PyObject *_mysql_programming_error;
PyObject *_mysql_not_supported_error;
PyObject *_mysql_error_map;

bool is_connection_lost(error_code)
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

bool is_retryable_error(error_code)
{
    /// connection error
    if (error_code >= CR_SOCKET_CREATE_ERROR && error_code <= CR_SERVER_GONE_ERROR)
        return true;
    /// lost connection or dead-lock
    if (error_code == CR_SERVER_LOST || error_code == ER_LOCK_DEADLOCK)
        return true;

    return false;
}

PyObject*
_mysql_set_exception(PyObject* cls, int code, const char* message)
{
    PyObject *py_code = PyLong_FromLong(code);
    PyObject *py_message = PyString_FromString(message);
    if (cls == NULL) {
        cls = PyDict_GetItem(_mysql_error_map, py_code);
        if (!cls)
            cls = code < ER_ERROR_FIRST ? _mysql_internal_error : _mysql_operational_error;
    }

    PyObject* exc = PyObject_CallFunction(cls, "OO", py_code, py_message);
    if (exc) {
        PyObject_SetAttrString(exc, "code", py_code);
        PyObject_SetAttrString(exc, "message", py_message);
        PyErr_SetObject(cls, exc);
    }
    Py_XDECREF(py_code);
    Py_XDECREF(py_message);
    return NULL;
}

PyObject*
_mysql_exception(_mysql_connection_object *c)
{
    PyObject *error_class;
    int error_code;

    if (!_mysql_server_init_done || !c) {
        error_class = _mysql_internal_error;
        return _mysql_set_exception(_mysql_internal_error, -1, "server not initialized");
    }

    error_code = mysql_errno(&(c->connection));

    TRACE2("%p, %d", c, error_code);
    if (is_connection_lost(error_code)) {
        c->connected = 0;
    }

    if (!error_code) {
        return _mysql_set_exception(_mysql_interface_error, -1, "unknown error");
    }

    if (error_code > CR_MAX_ERROR) {
        return _mysql_set_exception(_mysql_interface_error, -1, "error totally whack");
    }

    return _mysql_set_exception(NULL, error_code, mysql_error(&(c->connection)));
}

const char _mysql_is_retryable_error__doc__[] =
"Return True if error is retryable, otherwise False\n";

PyObject *
_mysql_is_retryable_error(PyObject* self, PyObject* args)
{
    int result = 0;
    PyObject* exc;

    if (!PyArg_ParseTuple(args, "O", &exc))
        return NULL;

    if (PyObject_IsInstance(exc, _mysql_error)) {
        PyObject* code = PyObject_GetAttrString(exc, "code");
        if (code && is_retryable_error(PyLong_AsLong(code)))
            result = 1;
    }
    return PyBool_FromLong(result);
}


const char _mysql_exceptions__doc__[] =
"All Exceptions according to DB API 2.0\n";

static PyMethodDef
_mysql_exceptions_methods[] = {
    {
        "is_retryable",
        (PyCFunction)_mysql_is_retryable_error,
        METH_VARARGS,
        _mysql_is_retryable_error__doc__
    },
    {NULL, NULL} /* sentinel */
};

#ifdef PY3K
PyModuleDef _mysql_exceptions_module =
{
    PyModuleDef_HEAD_INIT,
    STRINGIFY(MODULE_NAME) ".exceptions", /* name of module */
    _mysql_exceptions__doc__,             /* module documentation, may be NULL */
    -1,                                   /* size of per-interpreter state of the module,
                                             or -1 if the module keeps state in global variables. */
    _mysql_exceptions_methods,
    NULL, NULL, NULL, NULL
};
#endif  // PY3K

int
_mysql_exceptions_add(PyObject* module)
{
    _mysql_standard_error = NULL;
    _mysql_warning = NULL;
    _mysql_error = NULL;
    _mysql_database_error = NULL;
    _mysql_interface_error = NULL;
    _mysql_data_error = NULL;
    _mysql_operational_error = NULL;
    _mysql_integrity_error = NULL;
    _mysql_internal_error = NULL;
    _mysql_programming_error = NULL;
    _mysql_not_supported_error = NULL;
    _mysql_error_map = NULL;

    PyObject * warning_base = NULL, *exceptions = NULL;
#ifdef PY3K
    exceptions = PyModule_Create(&_mysql_exceptions_module);
#else
    exceptions = Py_InitModule3(STRINGIFY(MODULE_NAME) ".exceptions", _mysql_exceptions_methods, _mysql_exceptions__doc__);
#endif
    if (!exceptions)
        return -1;

    if (! (_mysql_standard_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.StandardError",
        "Exception related to operation with MySQL.",
        PyExc_Exception,
        0))) goto on_error;


    if (!(warning_base = Py_BuildValue("(OO)", PyExc_Warning, _mysql_standard_error))) goto on_error;

    if (!(_mysql_warning = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.Warning",
        "Exception raised for important warnings like data truncations while inserting, etc.",
        warning_base,
        0))) goto on_error;

    Py_DECREF(warning_base);
    warning_base = NULL;

    if (!(_mysql_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.Error",
        "Exception that is the base class of all other error exceptions (not Warning).",
        _mysql_standard_error,
        0))) goto on_error;

    if (!(_mysql_interface_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.InterfaceError",
        "Exception raised for errors that are related to the database interface rather than the database itself.",
        _mysql_error,
        0))) goto on_error;

    if (!(_mysql_database_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.DatabaseError",
        "Exception raised for errors that are related to the database.",
        _mysql_error,
        0))) goto on_error;

    if (!(_mysql_data_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.DataError",
        "Exception raised for errors that are due to problems with the \n" \
        "processed data like division by zero, numeric value out of range, etc.",
        _mysql_database_error,
        0))) goto on_error;


    if (!(_mysql_operational_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.OperationalError",
        "Exception raised for errors that are related to the database's\n" \
        "operation and not necessarily under the control of the programmer," \
        "e.g. an unexpected disconnect occurs, the data source name is not" \
        "found, a transaction could not be processed, a memory allocation" \
        "error occurred during processing, etc.",
        _mysql_database_error,
        0))) goto on_error;


    if (!(_mysql_integrity_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.IntegrityError",
        "Exception raised when the relational integrity of the database" \
        "is affected, e.g. a foreign key check fails, duplicate key, etc.",
        _mysql_database_error,
        0))) goto on_error;


    if (!(_mysql_internal_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.InternalError",
        "Exception raised when the database encounters an internal" \
        "error, e.g. the cursor is not valid anymore, the transaction is out of sync, etc.",
        _mysql_database_error,
        0))) goto on_error;

    if (!(_mysql_programming_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.ProgrammingError",
        "Exception raised for programming errors, e.g. table not found" \
        "or already exists, syntax error in the SQL statement, wrong number" \
        "of parameters specified, etc.",
        _mysql_database_error,
        0))) goto on_error;


    if (!(_mysql_not_supported_error = PyErr_NewExceptionWithDoc(
        STRINGIFY(MODULE_NAME) ".exceptions.NotSupportedError",
        "Exception raised in case a method or database API was used" \
        "which is not supported by the database, e.g. requesting a" \
        ".rollback() on a connection that does not support transaction or" \
        "has transactions turned off.",
        _mysql_database_error,
        0))) goto on_error;

    if (PyModule_AddObject(exceptions, "StandardError", _mysql_standard_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "Warning", _mysql_warning) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "Error", _mysql_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "DatabaseError", _mysql_database_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "InterfaceError", _mysql_interface_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "DataError", _mysql_data_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "OperationalError", _mysql_operational_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "IntegrityError", _mysql_integrity_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "InternalError", _mysql_internal_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "ProgrammingError", _mysql_programming_error) < 0) goto on_error;
    if (PyModule_AddObject(exceptions, "NotSupportedError", _mysql_not_supported_error) < 0) goto on_error;

    if (!(_mysql_error_map = PyDict_New())) goto on_error;

    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_DB_CREATE_EXISTS), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_SYNTAX_ERROR), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_PARSE_ERROR), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_NO_SUCH_TABLE), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_WRONG_DB_NAME), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_WRONG_TABLE_NAME), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_FIELD_SPECIFIED_TWICE), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_INVALID_GROUP_FUNC_USE), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_UNSUPPORTED_EXTENSION), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_TABLE_MUST_HAVE_COLUMNS), _mysql_programming_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_CANT_DO_THIS_DURING_AN_TRANSACTION), _mysql_programming_error);

    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_WARN_DATA_TRUNCATED), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_WARN_NULL_TO_NOTNULL), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_WARN_DATA_OUT_OF_RANGE), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_NO_DEFAULT), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_PRIMARY_CANT_HAVE_NULL), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_DATA_TOO_LONG), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_DATETIME_FUNCTION_OVERFLOW), _mysql_data_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_PRIMARY_CANT_HAVE_NULL), _mysql_data_error);

    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_DUP_ENTRY), _mysql_integrity_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_NO_REFERENCED_ROW), _mysql_integrity_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_NO_REFERENCED_ROW_2), _mysql_integrity_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_ROW_IS_REFERENCED), _mysql_integrity_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_ROW_IS_REFERENCED_2), _mysql_integrity_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_CANNOT_ADD_FOREIGN), _mysql_integrity_error);

    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_WARNING_NOT_COMPLETE_ROLLBACK), _mysql_not_supported_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_NOT_SUPPORTED_YET), _mysql_not_supported_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_FEATURE_DISABLED), _mysql_not_supported_error);
    PyDict_SetItem(_mysql_error_map, PyLong_FromLong(ER_UNKNOWN_STORAGE_ENGINE), _mysql_not_supported_error);

    if (PyModule_AddObject(module, "exceptions", exceptions) < 0) goto on_error;

    return 0;
    
on_error:
    Py_XDECREF(_mysql_standard_error);
    Py_XDECREF(_mysql_warning);
    Py_XDECREF(_mysql_error);
    Py_XDECREF(_mysql_database_error);
    Py_XDECREF(_mysql_interface_error);
    Py_XDECREF(_mysql_data_error);
    Py_XDECREF(_mysql_operational_error);
    Py_XDECREF(_mysql_integrity_error);
    Py_XDECREF(_mysql_internal_error);
    Py_XDECREF(_mysql_programming_error);
    Py_XDECREF(_mysql_not_supported_error);
    Py_XDECREF(_mysql_error_map);
    Py_XDECREF(warning_base);
    Py_XDECREF(exceptions);
    return -1;
}