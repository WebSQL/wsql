/* -*- mode: C; indent-tabs-mode: t; c-basic-offset: 8; -*- */

#include "mysqlmod.h"

static char _mysql_field_object__doc__[] =
"";

int
_mysql_field_object__init__(
	_mysql_field_object *self,
	PyObject *args,
	PyObject *kwargs)
{
	static char *kwlist[] = {"result", "index", NULL};
	_mysql_result_object *result = NULL;
	MYSQL_FIELD *field;
	unsigned int index;
	TRACE1("%p", self);
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", kwlist, &result, &index))
		return -1;

	self->index = index;
	field = mysql_fetch_field_direct(result->result, index);
	if (!field)
	    return -1;
	self->field = *field;
	return 0;
}

static int
_mysql_field_object_clear(
	_mysql_field_object *self)
{
    TRACE1("%p", self);
	return 0;
}

static void
_mysql_field_object_dealloc(
	_mysql_field_object *self)
{
    TRACE1("%p", self);
    _mysql_field_object_clear(self);
	Py_FREE(self);
}

static PyObject *
_mysql_field_object_repr(
	_mysql_field_object *self)
{
	return PyString_FromFormat("<_mysql.Field object at %p>", self);
}

static PyMethodDef _mysql_field_object_methods[] = {
	{NULL,              NULL} /* sentinel */
};

static struct PyMemberDef _mysql_field_object_members[] = {
	{
		"name",
		T_STRING,
		offsetof(_mysql_field_object, field.name),
		READONLY,
		"The name of the field. If the field was given\n" \
        "an alias with an AS clause, the value of name is the alias."
	},
	{
		"org_name",
		T_STRING,
		offsetof(_mysql_field_object, field.org_name),
		READONLY,
		"The name of the field. Aliases are ignored."
	},
	{
		"table",
		T_STRING,
		offsetof(_mysql_field_object, field.table),
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
		offsetof(_mysql_field_object, field.org_table),
		READONLY,
		"The name of the table. Aliases are ignored.\n" \
        "If the column is selected from a view, org_table names the underlying table.\n"
	},
	{
		"db",
		T_STRING,
		offsetof(_mysql_field_object, field.db),
		READONLY,
		"The name of the database that the field comes from.\n" \
        "If the field is a calculated field, db is an empty string."
	},
	{
		"catalog",
		T_STRING,
		offsetof(_mysql_field_object, field.catalog),
		READONLY,
		"The catalog name. This value is always \"def\"."
	},
	{
		"length",
		T_ULONG,
		offsetof(_mysql_field_object, field.length),
		READONLY,
		"The width of the field as specified in the table definition.\n"
	},
	{
		"max_length",
		T_ULONG,
		offsetof(_mysql_field_object, field.max_length),
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
		offsetof(_mysql_field_object, field.decimals),
		READONLY,
		"The number of decimals for numeric fields.\n"
	},
	{
		"charsetnr",
		T_UINT,
		offsetof(_mysql_field_object, field.charsetnr),
		READONLY,
		"The character set number for the field."
	},
	{
		"flags",
		T_UINT,
		offsetof(_mysql_field_object, field.flags),
		READONLY,
		"Different bit-flags for the field.\n" \
        "The bits are enumerated in MySQLdb.constants.FLAG.\n" \
        "The flags value may have zero or more of these bits set.\n"
	},
	{
		"type",
		T_UINT,
		offsetof(_mysql_field_object, field.type),
		READONLY,
		"The type of the field. The type values are enumerated in MySQLdb.constants.FIELD_TYPE.\n"
	},
	{NULL} /* Sentinel */
};

PyTypeObject _mysql_field_object_t = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_mysql.Field",                                                /* tp_name */
    sizeof(_mysql_field_object),                                    /* tp_basicsize */
    0,                                                             /* tp_itemsize */
    (destructor)_mysql_field_object_dealloc,                        /* tp_dealloc */
    0,                                                             /* tp_print */
    0,                                                             /* tp_getattr */
    0,                                                             /* tp_setattr */
    0,                                                             /* tp_reserved */
    (reprfunc)_mysql_field_object_repr,                             /* tp_repr */
    0,                                                             /* tp_as_number */
    0,                                                             /* tp_as_sequence */
    0,                                                             /* tp_as_mapping */
    0,                                                             /* tp_hash  */
    0,                                                             /* tp_call */
    0,                                                             /* tp_str */
    0,                                                             /* tp_getattro */
    0,                                                             /* tp_setattro */
    0,                                                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                                            /* tp_flags */
    _mysql_field_object__doc__,                                     /* tp_doc */
    0,                                                             /* tp_traverse */
    (inquiry)_mysql_field_object_clear,                             /* tp_clear */
    0,                                                             /* tp_richcompare */
    0,                                                             /* tp_weaklistoffset */
    0,                                                             /* tp_iter */
    0,                                                             /* tp_iternext */
    _mysql_field_object_methods,                                    /* tp_methods */
    _mysql_field_object_members,                                    /* tp_members */
    0,                                                             /* tp_getset */
    0,                                                             /* tp_base */
    0,                                                             /* tp_dict */
    0,                                                             /* tp_descr_get */
    0,                                                             /* tp_descr_set */
    0,                                                             /* tp_dictoffset */
    (initproc)_mysql_field_object__init__,                          /* tp_init */
    PyType_GenericAlloc,                                           /* tp_alloc */
    (newfunc)_PyObject_NewVar,                                     /* tp_new */
    PyObject_Del
};
