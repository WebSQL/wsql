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

#ifndef __WSQL_COMPAT_H__
#define __WSQL_COMPAT_H__

#define PY_SSIZE_T_CLEAN 1

#include <Python.h>

#define Py_ALLOC(s,t) (s *) t.tp_alloc(&t, 0)
#define Py_FREE(o) ((PyObject*)o)->ob_type->tp_free((PyObject*)o)

#if PY_MAJOR_VERSION >= 3
#define PY3K 1
#define PyString_Check PyUnicode_Check
#define PyString_FromStringAndSize PyUnicode_FromStringAndSize
#define PyString_FromString PyUnicode_FromString
#define PyString_FromFormat PyUnicode_FromFormat
#define PyString_AsString PyUnicode_AsUTF8
#define PyString_AsStringAndSize2 PyUnicode_AsUTF8AndSize
#define PyString_Type  PyUnicode_Type
#else
#define PyBytes_FromStringAndSize PyString_FromStringAndSize
#define _PyBytes_Resize _PyString_Resize
#define PyBytes_AS_STRING PyString_AS_STRING

inline char *PyString_AsStringAndSize2(PyObject *obj, Py_ssize_t *length) {
    char* buffer = NULL;
    PyString_AsStringAndSize(obj, &buffer, length);
    return buffer;
}

inline PyObject *PyErr_NewExceptionWithDoc(char *name, char *doc, PyObject *base, PyObject *dict) {
    PyObject *exception = PyErr_NewException(name, base, dict);
    if (exception) {
        PyObject* pydoc = PyString_FromString(doc);
        PyObject_SetAttrString(exception, "__doc__", pydoc);
        Py_DECREF(pydoc);
    }
    return exception;
}
#endif // PYTHON3

#endif //__WSQL_COMPAT_H__