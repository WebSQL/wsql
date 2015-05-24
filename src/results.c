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

#include "result.h"
#include "field.h"

static PyObject* wsql_result_get_fields(wsql_result *self, unsigned int num_fields)
{
    PyObject *args2 = NULL, *kwargs2 = NULL;
    PyObject *fields = NULL;
    wsql_field *field = NULL;
    unsigned int i;

    CHECK_RESULT(self, NULL);

    if (!(fields = PyTuple_New(num_fields)))
        return NULL;

    if (!(kwargs2 = PyDict_New()))
        goto on_error;

    for (i=0; i<num_fields; ++i)
    {
        if (!(args2 = Py_BuildValue("(Oi)", self, i)))
            goto on_error;

        if (!(field = Py_ALLOC(wsql_field, wsql_field_t)))
            goto on_error;

        if (wsql_field__init__(field, args2, kwargs2))
        {
            Py_DECREF(field);
            goto on_error;
        }

        Py_DECREF(args2);
        PyTuple_SET_ITEM(fields, i, (PyObject *)field);
    }
    Py_DECREF(kwargs2);
    return fields;

  on_error:
    Py_XDECREF(args2);
    Py_XDECREF(kwargs2);
    Py_XDECREF(fields);
    return NULL;
}

static const char wsql_result__doc__[] =
"result(connection, use=0) -- Result set from a query.\n" \
"Creating instances of this class directly is an excellent way to\n" \
"shoot yourself in the foot. If using wsql.connection directly,\n" \
"use connection.store_result() or connection.use_result() instead.\n" \
"If using wsql.Connection, this is done by the cursor class.\n" \
"Just forget you ever saw this. Forget... FOR-GET...";

int wsql_result__init__(wsql_result *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"connection", "use", NULL};
    MYSQL_RES *result;
    wsql_connection *connection = NULL;
    int use = 0;
    unsigned int n;

    TRACE1("%p", self);

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|i", kwlist, &connection, &use))
        return -1;

    Py_BEGIN_ALLOW_THREADS ;
    result = use ? mysql_use_result(&(connection->connection)) : mysql_store_result(&(connection->connection));
    self->result = result;
    self->more_rows = result ? 1 : 0;
    self->use = use;
    Py_END_ALLOW_THREADS;

    self->connection = (PyObject *) connection;
    Py_INCREF(self->connection);

    if (!result)
    {
        if (mysql_field_count(&(connection->connection)) == 0)
            return 0;

        wsql_raise_error(connection);
        return -1;
    }
    n = mysql_num_fields(result);
    self->num_fields = n;
    self->fields = wsql_result_get_fields(self, n);
    return 0;
}

static char wsql_result_description__doc__[] =
"the sequence of 7-tuples required by the DB-API for\n" \
"the Cursor.description attribute.\n";

static PyObject* wsql_result_get_description(wsql_result *self, void* closure)
{
    PyObject *result;
    MYSQL_FIELD *fields;
    unsigned int i, n;

    CHECK_RESULT(self, NULL);

    n = self->num_fields;

    if (!(result = PyTuple_New(n)))
        return NULL;

    fields = mysql_fetch_fields(self->result);

    for (i=0; i<n; ++i) {
        PyObject *field_desc;
        field_desc = Py_BuildValue("(siiiiii)",
                                   fields[i].name,
                                   (long) fields[i].type,
                                   (long) fields[i].max_length,
                                   (long) fields[i].length,
                                   (long) fields[i].length,
                                   (long) fields[i].decimals,
                                   (long) !(IS_NOT_NULL(fields[i].flags)));
        if (!field_desc)
            goto on_error;

        PyTuple_SET_ITEM(result, i, field_desc);
    }
    return result;

  on_error:
    Py_XDECREF(result);
    return NULL;
}


static PyObject* wsql_result_convert_row(wsql_result *self, MYSQL_ROW row)
{
    PyObject *v, *result = NULL;
    unsigned int i, n;
    unsigned long *length;

    if (!row)
    {
      if (mysql_errno(&(RESULT_CONNECTION(self)->connection)))
        return wsql_raise_error(RESULT_CONNECTION(self));

      self->more_rows = 0;
      Py_RETURN_NONE;
    }

    n = self->num_fields;
    if (!(result = PyTuple_New(n)))
        return NULL;

    length = mysql_fetch_lengths(self->result);

    for (i=0; i<n; ++i)
    {
        if (row[i])
        {
            v = PyBytes_FromStringAndSize(row[i], length[i]);
            if (!v) goto on_error;
        }
        else /* NULL */
        {
            v = Py_None;
            Py_INCREF(v);
        }
        PyTuple_SET_ITEM(result, i, v);
    }
    return result;

  on_error:
    Py_DECREF(result);
    return NULL;
}

static const char wsql_result_fetch_row__doc__[] =
"  Fetches one row as a tuple of strings.\n" \
"  NULL is returned as None.\n" \
"  A single None indicates the end of the result set.\n";

static PyObject* wsql_result_fetch_row(wsql_result *self)
{
    MYSQL_ROW row;

    CHECK_RESULT(self, NULL);

    if (!self->use)
    {
        row = mysql_fetch_row(self->result);
    }
    else
    {
        Py_BEGIN_ALLOW_THREADS;
        row = mysql_fetch_row(self->result);
        Py_END_ALLOW_THREADS;
    }
    return wsql_result_convert_row(self, row);
}

static int wsql_result_clear(wsql_result *self)
{
    TRACE1("%p", self);

    if (self->result)
    {
        mysql_free_result(self->result);
        self->result = NULL;
    }

    Py_XDECREF(self->fields);
    self->fields = NULL;

    Py_XDECREF(self->connection);
    self->connection = NULL;
    return 0;
}

static const char wsql_result_free__doc__[] =
"free internal structures, should be called after all results are fetched.";

static PyObject* wsql_result_free(wsql_result *self)
{
    TRACE2("%p: %p", self, self->result);
    CHECK_RESULT(self, NULL);

    if (!self->result)
    {
        PyErr_SetString(wsql_programming_error, "the result has been destroyed already.");
        return NULL;
    }

    mysql_free_result(self->result);
    self->result = NULL;
    Py_RETURN_NONE;
}

static PyObject* wsql_result__iter__(wsql_result *self, PyObject *unused)
{
    CHECK_RESULT(self, NULL);
    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject* wsql_result_next(wsql_result *self)
{
    PyObject *row;
    CHECK_RESULT(self, NULL);
    row = wsql_result_fetch_row(self);
    if (row == Py_None)
    {
        Py_DECREF(row);
        PyErr_SetString(PyExc_StopIteration, "");
        return NULL;
    }
    return row;
}

static const char wsql_result_num_rows__doc__[] =
"The number of rows in the result set. Note that if\n"
"use=1, this will not return a valid value until the entire result\n"
"set has been read.\n";

static PyObject* wsql_result_get_num_rows(wsql_result *self, void* closure)
{
    CHECK_RESULT(self, NULL);
    return PyLong_FromUnsignedLongLong(mysql_num_rows(self->result));
}

static const char wsql_result_data_seek__doc__[] =
"data_seek(n) -- seek to row n of result set";

static PyObject* wsql_result_data_seek(wsql_result *self, PyObject *args)
{
    unsigned int row;
    if (!PyArg_ParseTuple(args, "i:data_seek", &row))
        return NULL;

    CHECK_RESULT(self, NULL);
    mysql_data_seek(self->result, row);
    Py_RETURN_NONE;
}

static const char wsql_result_row_seek__doc__[] =
"row_seek(n) -- seek by offset n rows of result set";

static PyObject* wsql_result_row_seek(wsql_result *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"origin", NULL};

    int origin=SEEK_CUR;
    Py_ssize_t offset;

    MYSQL_ROW_OFFSET r;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "n|i:", kwlist, &offset, &origin))

    CHECK_RESULT(self, NULL);
    if (self->use)
    {
        PyErr_SetString(wsql_programming_error, "cannot be used with connection.get_result(use=True)");
        return NULL;
    }

    r = mysql_row_tell(self->result);

    switch (origin)
    {
    case SEEK_CUR:
        r += (size_t)offset;
        break;
    case SEEK_SET:
        r = self->result->data->data + (size_t)offset;
        break;
    default:
        PyErr_SetString(wsql_programming_error, "unsupported origin");
        return NULL;
    };

    mysql_row_seek(self->result, r);
    Py_RETURN_NONE;
}

static const char wsql_result_row_tell__doc__[] =
"row_tell() -- return the current row number of the result set.";

static PyObject* wsql_result_row_tell(wsql_result *self)
{
    MYSQL_ROW_OFFSET r;

    CHECK_RESULT(self, NULL);
    if (self->use)
    {
        PyErr_SetString(wsql_programming_error, "cannot be used with connection.use_result()");
        return NULL;
    }
    r = mysql_row_tell(self->result);
    return PyLong_FromLong(r - self->result->data->data);
}


#ifdef HAVE_ASYNCIO

static const char wsql_result_fetch_row_async__doc__[] =
  "Fetch a row from the current query set in a non-blocking manner.\n"
  "Returns a tuple of (status, row).  Keep calling this function to\n"
  "retrieve all rows; when row is None, If status is NET_ASYNC_NOT_READY,\n"
  "the descriptor should be waited on for more rows.  Otherwise, status\n"
  "is NET_ASYNC_COMPLETE, and the query is complete.\n";

static PyObject* wsql_result_fetch_row_async(wsql_result *self)
{
    PyObject *row = NULL, *result = NULL;
    MYSQL_ROW mysql_row;
    net_async_status status;

    CHECK_RESULT(self, NULL);

    status = mysql_fetch_row_nonblocking(self->result, &mysql_row);

    if (status == NET_ASYNC_NOT_READY)
    {
        row = Py_None;
        Py_INCREF(row);
    }
    else
    {
        row = wsql_result_convert_row(self, mysql_row);
    }

    if (row)
    {
        result = Py_BuildValue("(iO)", status, row);
        Py_DECREF(row);
        return result;
    }

  on_error:
    Py_XDECREF(row);
    return NULL;
}

static const char wsql_result_free_async__doc__[] =
"free internal structures, should be called after all results are fetched(async version).";

static PyObject* wsql_result_free_async(wsql_result *self)
{
    net_async_status status;
    CHECK_RESULT(self, NULL);

    if (!self->result) {
        PyErr_SetString(wsql_programming_error, "The result has been destroyed already.");
        return NULL;
    }

    status = mysql_free_result_nonblocking(self->result);
    if (status == NET_ASYNC_COMPLETE) {
        self->result = NULL;
    }
    return Py_BuildValue("(iO)", status, Py_None);
}

#endif //HAVE_ASYNCIO

static void wsql_result_dealloc(wsql_result *self)
{
    TRACE1("%p", self);
    wsql_result_clear(self);
    Py_FREE(self);
}

static PyObject* wsql_result_repr(wsql_result *self)
{
    char buf[300];
    sprintf(buf, "<" STRINGIFY(MODULE_NAME) ".Result object at %p>", self);
    return PyString_FromString(buf);
}

static PyMethodDef wsql_result_methods[] = {
    {
        "data_seek",
        (PyCFunction)wsql_result_data_seek,
        METH_VARARGS,
        wsql_result_data_seek__doc__
    },
    {
        "row_seek",
        (PyCFunction)wsql_result_row_seek,
        METH_VARARGS | METH_KEYWORDS,
        wsql_result_row_seek__doc__
    },
    {
        "row_tell",
        (PyCFunction)wsql_result_row_tell,
        METH_NOARGS,
        wsql_result_row_tell__doc__
    },
    {
        "fetch_row",
        (PyCFunction)wsql_result_fetch_row,
        METH_NOARGS,
        wsql_result_fetch_row__doc__
    },
    {
        "free",
        (PyCFunction)wsql_result_free,
        METH_NOARGS,
        wsql_result_free__doc__
    },

#ifdef HAVE_ASYNCIO
    {
        "fetch_row_async",
        (PyCFunction)wsql_result_fetch_row_async,
        METH_NOARGS,
        wsql_result_fetch_row_async__doc__
    },
    {
        "free_async",
        (PyCFunction)wsql_result_free_async,
        METH_NOARGS,
        wsql_result_free_async__doc__
    },
#endif
    {NULL,              NULL} /* sentinel */
};

static struct PyMemberDef wsql_result_members[] = {
    {
        "connection",
        T_OBJECT,
        offsetof(wsql_result, connection),
        READONLY,
        "Connection associated with result"
    },
    {
        "fields",
        T_OBJECT,
        offsetof(wsql_result, fields),
        READONLY,
        "Field metadata for result set"
    },
    {
        "num_fields",
        T_LONG,
        offsetof(wsql_result, num_fields),
        READONLY,
        "The number of fields (column) in the result."
    },
    {
        "use",
        T_INT,
        offsetof(wsql_result, use),
        READONLY,
        "True if mysql_use_result() was used; False if mysql_store_result() was used"
    },
    {
        "more_rows",
        T_INT,
        offsetof(wsql_result, more_rows),
        READONLY,
        "True if there is more rows, otherwise False"
    },
    {NULL} /* Sentinel */
};

static struct PyGetSetDef wsql_result_getset[]  = {
    {
        "num_rows",
        (getter)wsql_result_get_num_rows,
        NULL,
        wsql_result_num_rows__doc__,
        NULL
    },
    {
        "description",
        (getter)wsql_result_get_description,
        NULL,
        wsql_result_description__doc__,
        NULL
    },
    {NULL} /* Sentinel */
};


PyTypeObject wsql_result_t = {
    PyVarObject_HEAD_INIT(NULL, 0)
    STRINGIFY(MODULE_NAME) ".Result",   /* tp_name */
    sizeof(wsql_result),                /* tp_basicsize */
    0,                                  /* tp_itemsize */
    (destructor)wsql_result_dealloc,    /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    (reprfunc)wsql_result_repr,         /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash  */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                 /* tp_flags */
    wsql_result__doc__,                 /* tp_doc */
    0,                                  /* tp_traverse */
    (inquiry)wsql_result_clear,         /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    (getiterfunc) wsql_result__iter__,  /* tp_iter */
    (iternextfunc) wsql_result_next,    /* tp_iternext */
    wsql_result_methods,                /* tp_methods */
    wsql_result_members,                /* tp_members */
    wsql_result_getset,                 /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    (initproc)wsql_result__init__,      /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    (newfunc)_PyObject_NewVar,          /* tp_new */
    PyObject_Del                        /* tp_del */
};
