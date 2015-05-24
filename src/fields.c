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

#include "field.h"
#include "result.h"

static const char wsql_field__doc__[] =
"The MySQL field presentation.";

int wsql_field__init__(wsql_field *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"result", "index", NULL};

    wsql_result *result = NULL;
    MYSQL_FIELD *field;
    unsigned int index;
    TRACE1("%p", self);
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", kwlist, &result, &index))
        return -1;

    self->index = index;
    field = mysql_fetch_field_direct(result->result, index);
    if (!field)
    {
        PyErr_SetString(wsql_internal_error, "cannot get field from result");
        return -1;
    }

    self->field = *field;
    return 0;
}

static int wsql_field_clear(wsql_field *self)
{
    TRACE1("%p", self);
    return 0;
}

static void wsql_field_dealloc(wsql_field *self)
{
    TRACE1("%p", self);
    wsql_field_clear(self);
    Py_FREE(self);
}

static PyObject* wsql_field_repr(wsql_field *self)
{
    return PyString_FromFormat("<" STRINGIFY(MODULE_NAME) ".Field object at %p>", self);
}

static PyMethodDef wsql_field_methods[] = {
    {NULL,              NULL} /* sentinel */
};

static struct PyMemberDef wsql_field_members[] = {
    {
        "name",
        T_STRING,
        offsetof(wsql_field, field.name),
        READONLY,
        "The name of the field. If the field was given\n" \
        "an alias with an AS clause, the value of name is the alias."
    },
    {
        "org_name",
        T_STRING,
        offsetof(wsql_field, field.org_name),
        READONLY,
        "The name of the field. Aliases are ignored."
    },
    {
        "table",
        T_STRING,
        offsetof(wsql_field, field.table),
        READONLY,
        "The name of the table containing this field,\n" \
        "if it isn't a calculated field. For calculated fields,\n" \
        "the table value is an empty string. If the column is selected from a view,\n" \
        "table names the view. If the table or view was given an alias with an AS clause,\n" \
        "the value of table is the alias.\n"
    },
    {
        "org_table",
        T_STRING,
        offsetof(wsql_field, field.org_table),
        READONLY,
        "The name of the table. Aliases are ignored.\n" \
        "If the column is selected from a view, org_table names the underlying table.\n"
    },
    {
        "db",
        T_STRING,
        offsetof(wsql_field, field.db),
        READONLY,
        "The name of the database that the field comes from.\n" \
        "If the field is a calculated field, db is an empty string."
    },
    {
        "catalog",
        T_STRING,
        offsetof(wsql_field, field.catalog),
        READONLY,
        "The catalog name. This value is always \"def\"."
    },
    {
        "length",
        T_ULONG,
        offsetof(wsql_field, field.length),
        READONLY,
        "The width of the field as specified in the table definition.\n"
    },
    {
        "max_length",
        T_ULONG,
        offsetof(wsql_field, field.max_length),
        READONLY,
        "The maximum width of the field for the result set\n" \
        "(the length of the longest field value for the rows actually in the\n" \
        "result set). If you use conn.store_result(), this contains the\n" \
        "maximum length for the field. If you use conn.use_result(),\n" \
        "the value of this variable is zero.\n"
    },
    {
        "decimals",
        T_UINT,
        offsetof(wsql_field, field.decimals),
        READONLY,
        "The number of decimals for numeric fields.\n"
    },
    {
        "charsetnr",
        T_UINT,
        offsetof(wsql_field, field.charsetnr),
        READONLY,
        "The character set number for the field."
    },
    {
        "flags",
        T_UINT,
        offsetof(wsql_field, field.flags),
        READONLY,
        "Different bit-flags for the field.\n" \
        "The bits are enumerated in MySQLdb.constants.FLAG.\n" \
        "The flags value may have zero or more of these bits set.\n"
    },
    {
        "type",
        T_UINT,
        offsetof(wsql_field, field.type),
        READONLY,
        "The type of the field. The type values are enumerated in MySQLdb.constants.FIELD_TYPE.\n"
    },
    {NULL} /* Sentinel */
};

PyTypeObject wsql_field_t = {
    PyVarObject_HEAD_INIT(NULL, 0)
    STRINGIFY(MODULE_NAME) ".Field",   /* tp_name */
    sizeof(wsql_field),                /* tp_basicsize */
    0,                                 /* tp_itemsize */
    (destructor)wsql_field_dealloc,    /* tp_dealloc */
    0,                                 /* tp_print */
    0,                                 /* tp_getattr */
    0,                                 /* tp_setattr */
    0,                                 /* tp_reserved */
    (reprfunc)wsql_field_repr,         /* tp_repr */
    0,                                 /* tp_as_number */
    0,                                 /* tp_as_sequence */
    0,                                 /* tp_as_mapping */
    0,                                 /* tp_hash  */
    0,                                 /* tp_call */
    0,                                 /* tp_str */
    0,                                 /* tp_getattro */
    0,                                 /* tp_setattro */
    0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                /* tp_flags */
    wsql_field__doc__,                 /* tp_doc */
    0,                                 /* tp_traverse */
    (inquiry)wsql_field_clear,         /* tp_clear */
    0,                                 /* tp_richcompare */
    0,                                 /* tp_weaklistoffset */
    0,                                 /* tp_iter */
    0,                                 /* tp_iternext */
    wsql_field_methods,                /* tp_methods */
    wsql_field_members,                /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    0,                                 /* tp_descr_get */
    0,                                 /* tp_descr_set */
    0,                                 /* tp_dictoffset */
    (initproc)wsql_field__init__,      /* tp_init */
    PyType_GenericAlloc,               /* tp_alloc */
    (newfunc)_PyObject_NewVar,         /* tp_new */
    PyObject_Del                       /* tp_del */
};
