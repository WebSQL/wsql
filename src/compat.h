#ifndef _MYSQL_PYTHON__COMPAT_H_
#define _MYSQL_PYTHON__COMPAT_H_

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
#error "only python3 compatible"
#define PyBytes_FromStringAndSize PyString_FromStringAndSize
#define _PyBytes_Resize _PyString_Resize
#define PyBytes_AS_STRING PyString_AS_STRING

#define PY_MOD_RETURN(m) return
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
#endif // Python3

#endif //_MYSQL_PYTHON__COMPAT_H_