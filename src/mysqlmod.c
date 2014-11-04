/* -*- mode: C; indent-tabs-mode: t; c-basic-offset: 8; -*- */

#include "mysqlmod.h"

int _mysql_server_init_done = 0;


static char _mysql_server_init__doc__[] =
"Initialize embedded server. If this client is not linked against\n" \
"the embedded server library, this function does nothing.\n" \
"\n" \
"args -- sequence of command-line arguments\n" \
"groups -- sequence of groups to use in defaults files\n";

static PyObject *
_mysql_server_init(
    PyObject *self,
    PyObject *args,
    PyObject *kwargs)
{
    static char *kwlist[] = {"args", "groups", NULL};
    char **cmd_args_c=NULL, **groups_c=NULL, *s;
    Py_ssize_t cmd_argc=0, i, groupc;
    PyObject *cmd_args=NULL, *groups=NULL, *item;

    if (_mysql_server_init_done) {
        PyErr_SetString(_mysql_programming_error, "already initialized");
        return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO", kwlist, &cmd_args, &groups))
        return NULL;

#if MYSQL_VERSION_ID >= 40000
    if (cmd_args) {
        if (!PySequence_Check(cmd_args)) {
            PyErr_SetString(PyExc_TypeError, "args must be a sequence");
            goto finish;
        }
        cmd_argc = PySequence_Size(cmd_args);
        if (cmd_argc == -1) {
            PyErr_SetString(PyExc_TypeError, "args could not be sized");
            goto finish;
        }
        cmd_args_c = (char **) PyMem_Malloc(cmd_argc * sizeof(char *));
        for (i=0; i< cmd_argc; i++) {
            item = PySequence_GetItem(cmd_args, i);
            s = PyString_AsString(item);
            Py_DECREF(item);
            if (!s) {
                PyErr_SetString(PyExc_TypeError, "args must contain strings");
                goto finish;
            }
            cmd_args_c[i] = s;
        }
    }
    if (groups) {
        if (!PySequence_Check(groups)) {
            PyErr_SetString(PyExc_TypeError, "groups must be a sequence");
            goto finish;
        }
        groupc = PySequence_Size(groups);
        if (groupc == -1) {
            PyErr_SetString(PyExc_TypeError, "groups could not be sized");
            goto finish;
        }
        groups_c = (char **) PyMem_Malloc((1+groupc)*sizeof(char *));
        for (i=0; i< groupc; i++) {
            item = PySequence_GetItem(groups, i);
            s = PyString_AsString(item);
            Py_DECREF(item);
            if (!s) {
                PyErr_SetString(PyExc_TypeError, "groups must contain strings");
                goto finish;
            }
            groups_c[i] = s;
        }
        groups_c[groupc] = (char *)NULL;
    }
    /* even though this may block, don't give up the interpreter lock
       so that the server can't be initialized multiple times. */
    if (mysql_server_init(cmd_argc, cmd_args_c, groups_c)) {
        _mysql_exception(NULL);
        goto finish;
    }
#endif
    _mysql_server_init_done = 1;
    Py_RETURN_NONE;
  finish:
    PyMem_Free(groups_c);
    PyMem_Free(cmd_args_c);
    return NULL;
}

static char _mysql_server_end__doc__[] =
"Shut down embedded server. If not using an embedded server, this\n" \
"does nothing.";

static PyObject *
_mysql_server_end(
    PyObject *self)
{
    if (_mysql_server_init_done) {
#if MYSQL_VERSION_ID >= 40000
        mysql_server_end();
#endif
        _mysql_server_init_done = 0;
        Py_RETURN_NONE;
    }
    return _mysql_exception(NULL);
}

#if MYSQL_VERSION_ID >= 32314
static char _mysql_thread_safe__doc__[] =
"Indicates whether the client is compiled as thread-safe.";

static PyObject *
_mysql_thread_safe(
    PyObject *self)
{

    CHECK_SERVER(NULL);
    return PyLong_FromLong((long)mysql_thread_safe());
}
#endif

static char _mysql_debug__doc__[] =
"Does a DBUG_PUSH with the given string.\n" \
"mysql_debug() uses the Fred Fish debug library.\n" \
"To use this function, you must compile the client library to\n" \
"support debugging.\n";

static PyObject *
_mysql_debug(
    PyObject *self,
    PyObject *args)
{
    char *debug;
    if (!PyArg_ParseTuple(args, "s", &debug))
        return NULL;
    mysql_debug(debug);
    Py_RETURN_NONE;
}

static PyObject *_mysql_NULL;

static char _mysql_get_client_info__doc__[] =
"get_client_info() -- Returns a string that represents\n" \
"the client library version.";

static PyObject *
_mysql_get_client_info(
    PyObject *self)
{
    CHECK_SERVER(NULL);
    return PyString_FromString(mysql_get_client_info());
}

static PyMethodDef
_mysql_methods[] = {
    {
        "connect",
        (PyCFunction)_mysql_connect,
        METH_VARARGS | METH_KEYWORDS,
        _mysql_connect__doc__
    },
    {
        "debug",
        (PyCFunction)_mysql_debug,
        METH_VARARGS,
        _mysql_debug__doc__
    },
    {
        "get_client_info",
        (PyCFunction)_mysql_get_client_info,
        METH_NOARGS,
        _mysql_get_client_info__doc__
    },
#if MYSQL_VERSION_ID >= 32314
    {
        "thread_safe",
        (PyCFunction)_mysql_thread_safe,
        METH_NOARGS,
        _mysql_thread_safe__doc__
    },
#endif
    {
        "server_init",
        (PyCFunction)_mysql_server_init,
        METH_VARARGS | METH_KEYWORDS,
        _mysql_server_init__doc__
    },
    {
        "server_end",
        (PyCFunction)_mysql_server_end,
        METH_NOARGS,
        _mysql_server_end__doc__
    },
    {NULL, NULL} /* sentinel */
};

#define QUOTE(X) _QUOTE(X)
#define _QUOTE(X) #X

static char _mysql__doc__[] =
"an adaptation of the MySQL C API (mostly)\n" \
"\n" \
"You probably are better off using MySQLdb instead of using this\n" \
"module directly.\n" \
"\n" \
"In general, renaming goes from mysql_* to _mysql.*. _mysql.connect()\n" \
"returns a connection object (MYSQL). Functions which expect MYSQL * as\n" \
"an argument are now methods of the connection object. A number of things\n" \
"return result objects (MYSQL_RES). Functions which expect MYSQL_RES * as\n" \
"an argument are now methods of the result object. Deprecated functions\n" \
"(as of 3.23) are NOT implemented.\n";

#ifdef PY3K
PyModuleDef _mysql_module =
{
    PyModuleDef_HEAD_INIT,
    "_mysql",            /* name of module */
    _mysql__doc__,      /* module documentation, may be NULL */
    -1,                  /* size of per-interpreter state of the module,
                            or -1 if the module keeps state in global variables. */
    _mysql_methods, NULL, NULL, NULL, NULL
};

#define PYENTRY_FUNC_NAME PyInit__mysql
#define PY_MOD_RETURN(m) return m
#else
#define PYENTRY_FUNC_NAME init_mysql
#define PY_MOD_RETURN(m) return
#endif  // PY3K

PyMODINIT_FUNC
PYENTRY_FUNC_NAME(void)
{
    PyObject *module = NULL;
#ifdef PY3K
    module = PyModule_Create(&_mysql_module);
#else
    module = Py_InitModule3("_mysql", _mysql_methods, _mysql__doc__);
#endif
    if (!module)
        PY_MOD_RETURN(NULL); /* this really should never happen */

    /* Populate final object settings */
    if (PyType_Ready(&_mysql_connection_object_t) < 0) goto error;
    if (PyType_Ready(&_mysql_result_object_t) < 0) goto error;
    if (PyType_Ready(&_mysql_field_object_t) < 0) goto error;

    /* Module constants */
    if (PyModule_AddObject(module, "version_info",
                           PyRun_String(QUOTE(version_info),
                                        Py_eval_input,
                                        PyModule_GetDict(module),
                                        PyModule_GetDict(module))) < 0) goto error;

    if (PyModule_AddStringConstant(module, "__version__", QUOTE(__version__)) < 0) goto error;

    if (_mysql_exceptions_add(module) < 0) goto error;
    if (_mysql_constants_add(module) < 0) goto error;

    PY_MOD_RETURN(module);
  error:
    Py_XDECREF(module);
    PY_MOD_RETURN(NULL);
}
