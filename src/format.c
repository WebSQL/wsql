/*
WSQL
====
An asynchronous python interface to wsql
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

#include "format.h"

// added local support for formatting bytes
// original pep  460

struct wsql_formatter_t {
    PyObject *args;
    Py_ssize_t args_len, args_pos;

    Py_ssize_t fmt_len, fmt_pos;
    char *fmt_data;
    PyObject *fmt;
    char* out_data;
    Py_ssize_t out_len, out_pos;
    PyObject *output;
};

struct wsql_format_arg_t {
    char ch;
    int sign;
};


static int wsql_format_resize_output(struct wsql_formatter_t* ctx, Py_ssize_t size)
{
    if (_PyBytes_Resize(&(ctx->output), size) < 0) {
        return -1;
    }
    ctx->out_len = PyBytes_GET_SIZE(ctx->output);
    ctx->out_data = PyBytes_AS_STRING(ctx->output);
    return 0;
}


static int wsql_format_write_char(struct wsql_formatter_t* ctx, char c) {
    if (ctx->out_pos == ctx->out_len && wsql_format_resize_output(ctx, ctx->out_len << 1) < 0)
      return -1;

    ctx->out_data[ctx->out_pos++] = c;
    return 0;
}

static int wsql_format_write_object(struct wsql_formatter_t* ctx, PyObject* o) {
    PyObject* bytes = NULL;
    Py_ssize_t i;

    if (PyLong_Check(o))
    {
        o = PyNumber_ToBase(o, 10);
        if (o == NULL)
            return -1;
        bytes = PyUnicode_AsASCIIString(o);
        Py_DECREF(o);
    }
    else if (PyUnicode_Check(o))
    {
        bytes = PyUnicode_AsASCIIString(o);
    }
    else if (PyBytes_Check(o))
    {
        bytes = o;
        Py_INCREF(o);
    }
    else
    {
        o = PyObject_Str(o);
        if (o == NULL)
            return -1;
        bytes = PyUnicode_AsASCIIString(o);
        Py_DECREF(o);
    }

    if (bytes == NULL)
        return -1;

    Py_ssize_t len = PyBytes_GET_SIZE(bytes);

    if (ctx->out_pos + len > ctx->out_len && wsql_format_resize_output(ctx, (ctx->out_len + len) << 1) < 0)
      return -1;

    char* data = PyBytes_AS_STRING(bytes);

    for (i = 0; i < len; ++i, ++ctx->out_pos)
        ctx->out_data[ctx->out_pos] = data[i];

    Py_DECREF(bytes);
    return 0;
}

static PyObject* wsql_format_next_arg(struct wsql_formatter_t *ctx)
{
    Py_ssize_t args_pos = ctx->args_pos;

    if (args_pos < ctx->args_len)
    {
        ++ctx->args_pos;
        return PyTuple_GetItem(ctx->args, args_pos);
    }
    PyErr_SetString(PyExc_TypeError, "not enough arguments for format string");
    return NULL;
}

static int wsql_format_float(PyObject *v, struct wsql_format_arg_t *arg, PyObject **p_output)
{
    char *p;
    double x;

    x = PyFloat_AsDouble(v);
    if (x == -1.0 && PyErr_Occurred())
        return -1;

    p = PyOS_double_to_string(x, arg->ch, 6, 0, NULL);
    if (p == NULL)
        return -1;

    *p_output = PyBytes_FromString(p);
    PyMem_Free(p);
    return 0;
}

static PyObject* wsql_format_long(PyObject *val, struct wsql_format_arg_t *arg) {
    PyObject *result = NULL, *tmp = NULL;
    char *buf;
    Py_ssize_t i;
    Py_ssize_t llen;
    int type = arg->ch;

    assert(PyLong_Check(val));

    switch (type) {
    default:
        assert(!"'type' not in [diuoxX]");
    case 'd':
    case 'i':
    case 'u':
        /* int and int subclasses should print numerically when a numeric */
        /* format code is used (see issue18780) */
        result = PyNumber_ToBase(val, 10);
        break;
    case 'o':
        result = PyNumber_ToBase(val, 8);
        break;
    case 'x':
    case 'X':
        result = PyNumber_ToBase(val, 16);
        break;
    }
    if (!result)
        return NULL;

    tmp = PyUnicode_AsASCIIString(result);
    Py_DECREF(result);
    result = tmp;

    if (!result)
        return NULL;

    buf = PyBytes_AS_STRING(result);
    llen = PyBytes_GET_SIZE(result);

    if (llen > INT_MAX)
    {
        Py_DECREF(result);
        PyErr_SetString(PyExc_ValueError, "string too large in _PyBytes_FormatLong");
        return NULL;
    }

    /* Fix up case for hex conversions. */
    if (type == 'X')
    {
        /* Need to convert all lower case letters to upper case.
           and need to convert 0x to 0X (and -0x to -0X). */
        for (i = 0; i < llen; ++i)
        {
            if (buf[i] >= 'a' && buf[i] <= 'x')
                buf[i] -= 'a'-'A';
        }
    }
    return result;
}

/* Format an integer or a float as an integer.
 * Return 1 if the number has been formatted into the writer,
 *        0 if the number has been formatted into *p_output
 *       -1 and raise an exception on error */
static int wsql_format_long_main(PyObject *v, struct wsql_format_arg_t *arg, PyObject **p_output)
{
    PyObject *iobj, *res;
    char type = (char)arg->ch;

    if (!PyNumber_Check(v))
        goto on_error;

    /* make sure number is a type of integer for o, x, and X */
    if (!PyLong_Check(v))
    {
        if (type == 'o' || type == 'x' || type == 'X')
        {
            iobj = PyNumber_Index(v);
            if (iobj == NULL)
            {
                if (PyErr_ExceptionMatches(PyExc_TypeError))
                    goto on_error;
                return -1;
            }
        }
        else
        {
            iobj = PyNumber_Long(v);
            if (iobj == NULL )
            {
                if (PyErr_ExceptionMatches(PyExc_TypeError))
                    goto on_error;
                return -1;
            }
        }
        assert(PyLong_Check(iobj));
    }
    else
    {
        iobj = v;
        Py_INCREF(iobj);
    }

    res = wsql_format_long(iobj, arg);
    Py_DECREF(iobj);
    if (res == NULL)
        return -1;
    *p_output = res;
    return 0;

on_error:
    switch(type)
    {
        case 'o':
        case 'x':
        case 'X':
            PyErr_Format(PyExc_TypeError,
                    "%%%c format: an integer is required, "
                    "not %.200s",
                    type, Py_TYPE(v)->tp_name);
            break;
        default:
            PyErr_Format(PyExc_TypeError,
                    "%%%c format: a number is required, "
                    "not %.200s",
                    type, Py_TYPE(v)->tp_name);
            break;
    }
    return -1;
}

/* Format one argument. Supported conversion specifiers:
   - "s" string
   - "i", "d", "u": int or float
   - "o", "x", "X": int
   - "e", "E", "f", "F", "g", "G": float
   - "c": int or str (1 character)
*/
static int wsql_format_arg_format(struct wsql_formatter_t *ctx, struct wsql_format_arg_t *arg)
{
    PyObject *v, *p_str = NULL;
    int ret;

    if (arg->ch == '%')
    {
        if (wsql_format_write_char(ctx, '%') < 0)
            return -1;
        return 1;
    }

    v = wsql_format_next_arg(ctx);
    if (v == NULL)
        return -1;

    switch (arg->ch)
    {
    case 's':
        if (wsql_format_write_object(ctx, v) < 0)
            return -1;
        return 0;

    case 'i':
    case 'd':
    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (wsql_format_long_main(v, arg, &p_str) < 0)
            return -1;
        arg->sign = 1;
        break;

    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
       if (wsql_format_float(v, arg, &p_str) < 0)
                return -1;
        break;

    default:
        PyErr_Format(PyExc_ValueError,
                     "unsupported format character '%c' (0x%x) "
                     "at index %zd",
                     (31 <= arg->ch && arg->ch <= 126) ? (char)arg->ch : '?',
                     (int)arg->ch,
                     ctx->fmt_pos - 1);
        return -1;
    }
    if (p_str == NULL)
        return -1;

    ret = wsql_format_write_object(ctx, p_str);
    Py_DECREF(p_str);
    return ret;
}

/* Helper of PyUnicode_Format(): format one arg.
   Return 0 on success, raise an exception and return -1 on error. */
static int wsql_format_arg(struct wsql_formatter_t *ctx)
{
    struct wsql_format_arg_t arg;

    arg.ch = ctx->fmt_data[ctx->fmt_pos++];
    arg.sign = 0;

    if (wsql_format_arg_format(ctx, &arg) < 0)
        return -1;

    return 0;
}


char wsql_format__doc__[] =
"format query, style like printf.\n" \
"Non-standard.\n";


PyObject* wsql_format(PyObject *self, PyObject *args)
{
    struct wsql_formatter_t ctx;
    char c;

    if (!PyArg_ParseTuple(args, "OO:format", &ctx.fmt, &ctx.args))
        return NULL;

    if (!PyBytes_Check(ctx.fmt))
    {
        PyErr_SetString(PyExc_TypeError, "format should be bytes");
        return NULL;
    }

    if (!PyTuple_Check(ctx.args))
    {
        PyErr_SetString(PyExc_TypeError, "arguments should be tuple");
        return NULL;
    }

    ctx.args_pos = 0;
    ctx.args_len = PyTuple_GET_SIZE(ctx.args);

    ctx.fmt_data = PyBytes_AS_STRING(ctx.fmt);
    ctx.fmt_len = PyBytes_GET_SIZE(ctx.fmt);
    ctx.fmt_pos = 0;

    ctx.out_pos = 0;
    ctx.output = PyBytes_FromStringAndSize(NULL, ctx.fmt_len << 1);
    ctx.out_data = PyBytes_AS_STRING(ctx.output);
    ctx.out_len = PyBytes_GET_SIZE(ctx.output);

    while (ctx.fmt_pos < ctx.fmt_len)
    {
        if (ctx.fmt_data[ctx.fmt_pos] != '%')
        {
            while (ctx.fmt_pos < ctx.fmt_len)
            {
                c = ctx.fmt_data[ctx.fmt_pos];
                if (c == '%')
                    break;

                wsql_format_write_char(&ctx, c);
                ++ctx.fmt_pos;
            }
        }
        else
        {

            ++ctx.fmt_pos;
            if (wsql_format_arg(&ctx) == -1)
                goto on_error;
        }
    }

    if (ctx.args_pos < ctx.args_len)
    {
        PyErr_SetString(PyExc_TypeError, "not all arguments converted during string formatting");
        goto on_error;
    }

    if (ctx.out_pos != ctx.out_len && _PyBytes_Resize(&(ctx.output), ctx.out_pos) < 0)
    {
       goto on_error;
    }

    return ctx.output;

  on_error:
    Py_XDECREF(ctx.output);
    return NULL;
}
