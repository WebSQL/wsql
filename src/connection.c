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
#include "result.h"

const char wsql_connect__doc__[] =
"Returns a WSQL connection. Exclusive use of keyword parameters strongly recommended.\n" \
"Consult the MySQL C API documentation for more details.\n" \
"\n " \
"param host: host to connect\n" \
"param user: user to connect as\n" \
"param password: password to use\n" \
"param database: database to use\n" \
"param port: TCP/IP port to connect to\n" \
"param socket_name: location of unix-socket or windows named pipe\n" \
"param connect_timeout: number of seconds to wait before the connection attempt fails.\n" \
"param compress: if set, gzip compression is enabled\n" \
"param init_command: command which is run once the connection is created\n" \
"param read_default_file: see the MySQL documentation for mysql_options()\n" \
"param read_default_group: see the MySQL documentation for mysql_options()\n" \
"param client_flag: client flags from MySQLdb.constants.CLIENT\n" \
"param load_infile: non-zero enables LOAD LOCAL INFILE, zero disables\n" \
"\n";


int wsql_connection__init__(wsql_connection *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {
        "host", "user", "password", "database",
        "port", "socket_name", "connect_timeout", "compress",
        "init_command", "read_default_file", "read_default_group", "client_flag",
        "ssl", "local_infile", "nonblocking", NULL
    };

    TRACE1("%p", self);

    MYSQL *conn = NULL;
    PyObject *ssl = NULL;
#if HAVE_OPENSSL
    char *key = NULL, *cert = NULL, *ca = NULL, *capath = NULL, *cipher = NULL;
#endif
    char *host = NULL, *user = NULL, *password = NULL,
         *database = NULL, *socket_name = NULL,
         *init_command=NULL, *read_default_file=NULL, *read_default_group=NULL;

    unsigned int port = 0, client_flag = 0, connect_timeout = 0;
    int nonblocking = 0, compress = -1, local_infile = -1;

    self->open = 0;
    self->autocommit = 0;
    self->connected = 0;
    CHECK_SERVER(-1);

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ssssIsIisssIOii:connect",
                     kwlist,
                     &host, &user, &password, &database,
                     &port, &socket_name, &connect_timeout, &compress,
                     &init_command, &read_default_file, &read_default_group,
                     &client_flag, &ssl, &local_infile, &nonblocking))
    {
        return -1;
    }


    if (nonblocking)
    {
#ifndef HAVE_ASYNCIO
        PyErr_SetString(wsql_not_supported_error, "The asynchronous operations does not supported.");
        return -1;
#endif
    }

#define GET_ITEM_STRING(k, t, d) \
{ t = PyMapping_GetItemString(d, #k);\
  if (t) { k = PyString_AsString(t); Py_DECREF(t);} \
  PyErr_Clear(); \
}

    if (ssl)
    {
#if HAVE_OPENSSL
        PyObject *value = NULL;
        GET_ITEM_STRING(ca, value, ssl);
        GET_ITEM_STRING(capath, value, ssl);
        GET_ITEM_STRING(cert, value, ssl);
        GET_ITEM_STRING(key, value, ssl);
        GET_ITEM_STRING(cipher, value, ssl);
#else
        PyErr_SetString(wsql_not_supported_error, "client library does not have SSL support");
        return -1;
#endif
    }
#undef GET_ITEM_STRING

    Py_BEGIN_ALLOW_THREADS ;
    conn = mysql_init(&(self->connection));

    if (compress != -1)
    {
        mysql_options(&(self->connection), MYSQL_OPT_COMPRESS, 0);
        client_flag |= CLIENT_COMPRESS;
    }
    if (connect_timeout)
        mysql_options(&(self->connection), MYSQL_OPT_CONNECT_TIMEOUT, (char *)&connect_timeout);
    if (init_command != NULL)
        mysql_options(&(self->connection), MYSQL_INIT_COMMAND, init_command);
    if (read_default_file != NULL)
        mysql_options(&(self->connection), MYSQL_READ_DEFAULT_FILE, read_default_file);
    if (read_default_group != NULL)
        mysql_options(&(self->connection), MYSQL_READ_DEFAULT_GROUP, read_default_group);
    if (local_infile != -1)
        mysql_options(&(self->connection), MYSQL_OPT_LOCAL_INFILE, (char *) &local_infile);

#ifdef _WIN32
    if (socket_name != NULL)
        mysql_options(&(self->connection), MYSQL_OPT_NAMED_PIPE, 0);
#endif

#if HAVE_OPENSSL
    if (ssl)
        mysql_ssl_set(&(self->connection), key, cert, ca, capath, cipher);
#endif

    if (nonblocking)
    {
#ifdef HAVE_ASYNCIO
        if (!mysql_real_connect_nonblocking_init(&(self->connection),
                host, user, password, database, port, socket_name, client_flag))
        {
            conn = NULL;
        }
#endif
    }
    else
    {
        conn = mysql_real_connect(&(self->connection), host, user, password, database, port, socket_name, client_flag);
    }

    self->autocommit = self->connection.server_capabilities & CLIENT_TRANSACTIONS ? 1 : 0;
    Py_END_ALLOW_THREADS ;

    if (conn == NULL)
    {
        wsql_raise_error(self);
        return -1;
    }

    self->open = 1;
    self->connected = 1;
    return 0;
}


PyObject *wsql_connect(PyObject *self, PyObject *args, PyObject *kwargs)
{
    wsql_connection *c = NULL;
    if (!(c = Py_ALLOC(wsql_connection, wsql_connection_t)))
        return NULL;

    if (wsql_connection__init__(c, args, kwargs) < 0)
    {
        Py_DECREF(c);
        c = NULL;
    }
    return (PyObject*)c;
}

static int wsql_connection_clear(wsql_connection *self)
{
    TRACE1("%p", self);
    return 0;
}


char wsql_connection_escape_string__doc__[] =
"escape_string(s) -- quote any SQL-interpreted characters in string s.\n " \
"If you want quotes around your value, use string_literal(s) instead.\n";

static PyObject* wsql_connection_escape_string(wsql_connection *self, PyObject *args)
{
    PyObject *output;
    Py_ssize_t input_size;
    unsigned long output_size;
    char* input_string;

    if (!PyArg_ParseTuple(args, "s#:escape", &input_string, &input_size))
        return NULL;

    if (!(output = PyBytes_FromStringAndSize(NULL, input_size << 1)))
        return PyErr_NoMemory();

    output_size = mysql_real_escape_string(&(self->connection), PyBytes_AS_STRING(output), input_string, input_size);
    if (output_size != input_size << 1 && _PyBytes_Resize(&output, output_size) < 0)
        return NULL;

    return output;
}

char wsql_connection_string_quote__doc__[] =
"quote(s) -- converts string s into a SQL string literal.\n" \
"This means, any special SQL characters are escaped, and it is enclosed\n" \
"within single quotes. In other words, it performs: \n" \
"\"'%s'\" % escape(s)\n";

static PyObject* wsql_connection_string_quote(wsql_connection *self, PyObject *args)
{
    PyObject *output;
    Py_ssize_t input_size;
    unsigned long output_size;
    char *input_string, *output_string;
    if (!PyArg_ParseTuple(args, "s#:quote", &input_string, &input_size))
        return NULL;

    if (!(output = PyBytes_FromStringAndSize(NULL, (input_size << 1) + 2)))
        return PyErr_NoMemory();

    output_string = PyBytes_AS_STRING(output);
    output_size = mysql_real_escape_string(&(self->connection), output_string + 1, input_string, input_size);
    output_string[0] = output_string[output_size + 1] = '\'';
    if (output_size != input_size << 1 && _PyBytes_Resize(&output, output_size + 2) < 0)
        return NULL;

    return output;
}

static const char wsql_connection_close__doc__[] =
"Close the connection. No further activity possible.";

static PyObject* wsql_connection_close(wsql_connection *self)
{
    if (self->open)
    {
        Py_BEGIN_ALLOW_THREADS
        mysql_close(&(self->connection));
        Py_END_ALLOW_THREADS
        self->open = 0;
        self->connected = 0;
    }
    else
    {
        PyErr_SetString(wsql_programming_error, "closing a closed connection.");
        return NULL;
    }
    wsql_connection_clear(self);
    Py_RETURN_NONE;
}

static const char wsql_connection_affected_rows__doc__ [] =
"Return number of rows affected by the last query.\n" \
"Non-standard. Use Cursor.rowcount.\n";

static PyObject* wsql_connection_get_affected_rows(wsql_connection *self, void *closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromUnsignedLongLong(mysql_affected_rows(&(self->connection)));
}

static const char wsql_connection_dump_debug_info__doc__[] =
"Instructs the server to write some debug information to the\n" \
"log. The connected user must have the process privilege for\n" \
"this to work. Non-standard.\n";

static PyObject* wsql_connection_dump_debug_info(wsql_connection *self)
{
    int error;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    error = mysql_dump_debug_info(&(self->connection));
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_autocommit__doc__[] =
"The autocommit mode. True means enable; False means disable.\n";

static PyObject* wsql_connection_get_autocommit(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyBool_FromLong(self->autocommit);
}

static int wsql_connection_set_autocommit(wsql_connection *self, PyObject *value, void *closure)
{
    int error = 0;
    int flag = PyLong_AsLong(value);
    if (PyErr_Occurred())
        return -1;

    CHECK_CONNECTION(self, -1);
    Py_BEGIN_ALLOW_THREADS
#if MYSQL_VERSION_ID >= 40100
    error = mysql_autocommit(&(self->connection), flag);
#else
    {
        char query[256];
        snprintf(query, 256, "SET AUTOCOMMIT=%d", flag);
        error = mysql_query(&(self->connection), query);
    }
#endif
    Py_END_ALLOW_THREADS
    if (error)
    {
        wsql_raise_error(self);
        return -1;
    }
    self->autocommit = flag ? 1 : 0;
    return 0;
}

static const char wsql_connection_set_sql_mode__doc__[] =
"Set the connection sql_mode. See MySQL documentation for legal values.\n"\
"Non-standard. It is better to set this when creating the connection\n"\
"using the sql_mode parameter.\n";

static PyObject* wsql_connection_set_sql_mode(wsql_connection *self, PyObject *args)
{
    int error = 0;
    char* sql_mode;

    if (!PyArg_ParseTuple(args, "s:set_sql_mode", &sql_mode))
        return NULL;

    CHECK_CONNECTION(self, NULL);

    Py_BEGIN_ALLOW_THREADS
    if (mysql_get_server_version(&(self->connection)) > 40100)
    {
        char query[256];
        snprintf(query, 256, "SET SESSION sql_mode='%s'", sql_mode);
        error = mysql_query(&(self->connection), query);
    }
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_commit__doc__[] =
"Commits the current transaction\n";

static PyObject* wsql_connection_commit(wsql_connection *self)
{
    int error;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
#if MYSQL_VERSION_ID >= 40100
    error = mysql_commit(&(self->connection));
#else
    error = mysql_query(&(self->connection), "COMMIT");
#endif
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_rollback__doc__[] =
"Rolls backs the current transaction.\n";

static PyObject* wsql_connection_rollback(wsql_connection *self)
{
    int error;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
#if MYSQL_VERSION_ID >= 40100
    error = mysql_rollback(&(self->connection));
#else
    error = mysql_query(&(self->connection), "ROLLBACK");
#endif
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_next_result__doc__[] =
"If more query results exist, next_result() reads the next query\n" \
"results and returns the status back to application.\n" \
"After calling next_result() the state of the connection is as if\n" \
"you had called query() for the next query. This means that you can\n" \
"now call store_result(), warning_count, affected_rows, and so forth. \n" \
"Returns True if there are more results.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_next_result(wsql_connection *self)
{
    int error;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
#if MYSQL_VERSION_ID >= 40100
    error = mysql_next_result(&(self->connection));
#else
    error = -1;
#endif
    Py_END_ALLOW_THREADS
    if (error > 0)
        return wsql_raise_error(self);

    return PyBool_FromLong(!error);
}

#if MYSQL_VERSION_ID >= 40100

static const char wsql_connection_set_server_option__doc__[] =
"set_server_option(option) -- Enables or disables an option for the connection.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_set_server_option(wsql_connection *self, PyObject *args)
{
    int error, flags=0;
    if (!PyArg_ParseTuple(args, "i", &flags))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    error = mysql_set_server_option(&(self->connection), flags);
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_sqlstate__doc__[] =
"The SQLSTATE error code for the last error.\n" \
"The error code consists of five characters. '00000' means \"no error.\"\n" \
"The values are specified by ANSI SQL and ODBC.\n" \
"For a list of possible values, see section 23 Error Handling in MySQL in the MySQL Manual.\n" \
"Note that not all MySQL errors are yet mapped to SQLSTATE's.\n" \
"The value 'HY000' (general error) is used for unmapped errors.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_sqlstate(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyString_FromString(mysql_sqlstate(&(self->connection)));
}

static const char wsql_connection_warning_count__doc__[] =
"The number of warnings generated during execution of the previous SQL statement.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_warning_count(wsql_connection *self, void *closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong(mysql_warning_count(&(self->connection)));
}

#endif

static const char wsql_connection_errno__doc__[] =
"The error code for the most recently invoked API function\n "\
"that can succeed or fail. A return value of zero means that no error occurred.\n";

static PyObject* wsql_connection_get_errno(wsql_connection *self, void *closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong((long)mysql_errno(&(self->connection)));
}

static const char wsql_connection_error__doc__[] =
"Contains the error message for the most recently invoked API function\n" \
"that can succeed or fail. An empty string ("") is returned if no error occurred.\n";

static PyObject* wsql_connection_get_error(wsql_connection *self, void *closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyString_FromString(mysql_error(&(self->connection)));
}

#if MYSQL_VERSION_ID >= 32303

static const char wsql_connection_change_user__doc__[] =
"Changes the user and causes the database specified by 'database' to\n" \
"become the default (current) database on the connection\n" \
"specified by mysql. In subsequent queries, this database is\n" \
"the default for table references that do not include an explicit database specifier.\n"
"This function was introduced in MySQL Version 3.23.3.\n" \
"Fails unless the connected user can be authenticated or if he doesn't have permission to use the database.\n"
"In this case the user and database are not changed.\n" \
"The 'database' parameter may be set to None if you don't want to have a default database.\n";

static PyObject* wsql_connection_change_user(wsql_connection *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "user", "password", "database", NULL } ;
    char *user, *pwd=NULL, *db=NULL;
    int error;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss:change_user", kwlist, &user, &pwd, &db))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    error = mysql_change_user(&(self->connection), user, pwd, db);
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}
#endif

static const char wsql_connection_charset__doc__[] =
"The default character set for the current connection.\n " \
"Non-standard.\n";

static PyObject* wsql_connection_get_charset(wsql_connection *self, void* closure)
{
    const char *s;

    CHECK_CONNECTION(self, NULL);
#if MYSQL_VERSION_ID >= 32321
    s = mysql_character_set_name(&(self->connection));
#else
    s = "latin1";
#endif
    return PyString_FromString(s);
}

static int wsql_connection_set_charset(wsql_connection *self, PyObject *arg, void* closure)
{
    int error = 0;
    const char* charset = PyString_AsString(arg);
    if (!charset)
        return -1;

    CHECK_CONNECTION(self, -1);

    Py_BEGIN_ALLOW_THREADS
    if (strcmp(mysql_character_set_name(&(self->connection)), charset) != 0) {
#if MYSQL_VERSION_ID >= 40100
        error = mysql_set_character_set(&(self->connection), charset);
#else
        char query[256];
        snprintf(query, 256, "SET NAMES %s", charset);
        error = mysql_query(&(self->connection), query);
#endif
    }

    Py_END_ALLOW_THREADS

    if (error)
    {
        wsql_raise_error(self);
        return -1;
    }
    return 0;
}

#if MYSQL_VERSION_ID >= 50010

static const char wsql_connection_charset_info__doc__[] =
"A dict with information about the current character set:\n" \
"\n" \
"collation - collation name\n" \
"name - character set name\n" \
"comment - comment or descriptive name\n" \
"dir - character set directory\n" \
"mbminlen - min. length for multibyte string\n" \
"mbmaxlen - max. length for multibyte string\n" \
"\n" \
"Not all keys may be present, particularly dir.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_charset_info(wsql_connection *self, void* closure)
{
    PyObject *result, *tmp;
    MY_CHARSET_INFO cs;

    CHECK_CONNECTION(self, NULL);

    mysql_get_character_set_info(&(self->connection), &cs);
    if (!(result = PyDict_New()))
        return NULL;

#define SET_ITEM_STRING(k, v) \
    if (v) { tmp = PyString_FromString(v); PyDict_SetItemString(result, k, tmp); Py_DECREF(tmp); }

    SET_ITEM_STRING("name", cs.csname);
    SET_ITEM_STRING("collation", cs.name);
    SET_ITEM_STRING("comment", cs.comment);
    SET_ITEM_STRING("dir", cs.dir);

#undef SET_ITEM_STRING

#define SET_ITEM_LONG(k, v) \
    { tmp = PyLong_FromLong(v); PyDict_SetItemString(result, k, tmp); Py_DECREF(tmp); }

    SET_ITEM_LONG("mbminlen", cs.mbminlen);
    SET_ITEM_LONG("mbmaxlen", cs.mbmaxlen);

#undef SET_ITEM_LONG

    return result;
}
#endif

static const char wsql_connection_host_info__doc__[] =
"Returns a string that represents the MySQL client library version.\n"
"Non-standard.\n";

static PyObject* wsql_connection_get_host_info(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyString_FromString(mysql_get_host_info(&(self->connection)));
}

static const char wsql_connection_proto_info__doc__[] =
"Returns an unsigned integer representing the protocol version\n" \
"used by the current connection.\n"
"Non-standard.\n";

static PyObject* wsql_connection_get_proto_info(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong((long)mysql_get_proto_info(&(self->connection)));
}

static const char wsql_connection_server_info__doc__[] =
"The server version number.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_server_info(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    const char* info = mysql_get_server_info(&(self->connection));
    if (!info)
    {
        PyErr_SetString(wsql_programming_error, "connection is not ready");
        return NULL;
    }
    return PyString_FromString(info);
}

static const char wsql_connection_info__doc__[] =
"The information about the most recently executed query.\n" \
"Use messages Cursor.messages.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_info(wsql_connection *self, void* closure)
{
    const char *s;
    CHECK_CONNECTION(self, NULL);
    s = mysql_info(&(self->connection));
    if (s)
        return PyString_FromString(s);
    Py_RETURN_NONE;
}

static const char wsql_connection_insert_id__doc__[] =
"The ID generated for an AUTO_INCREMENT column by the previous\n" \
"query. Use this function after you have performed an INSERT query into a\n" \
"table that contains an AUTO_INCREMENT field.\n" \
"Note that this returns 0 if the previous query does not generate an AUTO_INCREMENT value.\n" \
"If you need to save the value for later, \n" \
"be sure to call this immediately after the query that generates the value.\n" \
"The ID is updated after INSERT and UPDATE statements that generate an AUTO_INCREMENT value \n" \
"or that set a column value to LAST_INSERT_ID(expr).\n" \
"See section 6.3.5.2 Miscellaneous Functions in the MySQL documentation.\n" \
"Also note that the value of the SQL LAST_INSERT_ID() function always\n" \
"contains the most recently generated AUTO_INCREMENT value,\n" \
"and is not reset between queries because the value of that function is maintained in the server.\n";

static PyObject* wsql_connection_get_insert_id(wsql_connection *self, void* closure)
{
    my_ulonglong r;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    r = mysql_insert_id(&(self->connection));
    Py_END_ALLOW_THREADS
    return PyLong_FromUnsignedLongLong(r);
}

static const char wsql_connection_kill__doc__[] =
"Asks the server to kill the thread specified by pid.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_kill(wsql_connection *self, PyObject *args)
{
    unsigned long pid;
    int r;
    if (!PyArg_ParseTuple(args, "k:kill", &pid))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    r = mysql_kill(&(self->connection), pid);
    Py_END_ALLOW_THREADS
    if (r)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_field_count__doc__[] =
"The number of columns for the most recent query on the connection.\n" \
"Will probably give you bogus results on most cursor classes. Use Cursor.rowcount.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_field_count(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
#if MYSQL_VERSION_ID < 32224
    return PyLong_FromLong((long)mysql_num_fields(&(self->connection)));
#else
    return PyLong_FromLong((long)mysql_field_count(&(self->connection)));
#endif
}

static const char wsql_connection_ping__doc__[] =
"Checks whether or not the connection to the server is working.\n" \
"If it has gone down, an automatic reconnection is attempted.\n" \
"This function can be used by clients that remain idle for a long while,\n" \
"to check whether or not the server has closed the connection and reconnect if necessary.\n" \
"New in 1.2.2: Accepts an optional reconnect parameter.\n" \
"If True, then the client will attempt reconnection.\n" \
"Note that this setting is persistent. By default, this is on in MySQL<5.0.3, and off thereafter.\n" \
"You should assume that ping() performs an implicit rollback;\n" \
"use only when starting a new transaction. You have been warned.\n" \
"Non-standard\n";

static PyObject* wsql_connection_ping(wsql_connection *self, PyObject *args)
{
    int r, reconnect = -1;
    if (!PyArg_ParseTuple(args, "|I", &reconnect))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    if ( reconnect != -1 )
        self->connection.reconnect = reconnect;

    Py_BEGIN_ALLOW_THREADS
    r = mysql_ping(&(self->connection));
    Py_END_ALLOW_THREADS
    if (r)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_query__doc__[] =
"Execute a query. store_result() or use_result() will get the result set, if any.\n" \
"Use cursor() to create a cursor, then cursor.execute().\n" \
"Non-standard.\n";

static PyObject* wsql_connection_query(wsql_connection *self, PyObject *args)
{
    char *query;
    Py_ssize_t query_size;
    int  error;
    if (!PyArg_ParseTuple(args, "s#:query", &query, &query_size))
        return NULL;

    CHECK_CONNECTION(self, NULL);

    Py_BEGIN_ALLOW_THREADS
    error = mysql_real_query(&(self->connection), query, query_size);
    Py_END_ALLOW_THREADS
    if (error)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_select_db__doc__[] =
"Causes the database specified by db to become the default\n" \
"(current) database on the connection specified by mysql.\n" \
"In subsequent queries, this database is the default for table references that do not\n" \
"include an explicit database specifier.\n" \
"Fails unless the connected user can be authenticated as having permission to use the database.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_select_db(wsql_connection *self, PyObject *args)
{
    char *db;
    int r;
    if (!PyArg_ParseTuple(args, "s:select_db", &db))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    r = mysql_select_db(&(self->connection), db);
    Py_END_ALLOW_THREADS
    if (r)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_shutdown__doc__[] =
"Asks the database server to shut down. The connected user must\n" \
"have shutdown privileges. Non-standard.\n";

static PyObject* wsql_connection_shutdown(wsql_connection *self)
{
    int r;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    r = mysql_shutdown(&(self->connection)
#if MYSQL_VERSION_ID >= 40103
        , SHUTDOWN_DEFAULT
#endif
    );
    Py_END_ALLOW_THREADS
    if (r)
        return wsql_raise_error(self);

    Py_RETURN_NONE;
}

static const char wsql_connection_stat__doc__[] =
"Th information similar to that provided by the mysqladmin status command."
"This includes uptime in seconds and the number of running threads,\n" \
"questions, reloads, and open tables.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_stat(wsql_connection *self, void* closure)
{
    const char *s;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    s = mysql_stat(&(self->connection));
    Py_END_ALLOW_THREADS
    if (!s)
        return wsql_raise_error(self);

    return PyString_FromString(s);
}

static const char wsql_connection_get_result__doc__[] =
"Returns a result object. If use is True, mysql_use_result()\n" \
"is used; otherwise mysql_store_result() is used (the default).\n";

static PyObject* wsql_connection_get_result(wsql_connection *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"use", NULL};

    PyObject *args2 = NULL, *kwargs2 = NULL;
    wsql_result *result = NULL;
    int use = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i:get_result", kwlist, &use))
        return NULL;

    CHECK_CONNECTION(self, NULL);

    if (!(args2 = Py_BuildValue("(Oi)", self, use)))
        goto on_error;

    if (!(kwargs2 = PyDict_New()))
        goto on_error;

    if (!(result = Py_ALLOC(wsql_result, wsql_result_t)))
        goto on_error;

    if (wsql_result__init__(result, args2, kwargs2) < 0)
        goto on_error;

    Py_DECREF(args2);
    Py_DECREF(kwargs2);

    if (!(result->result))
    {
        Py_DECREF(result);
        Py_RETURN_NONE;
    }
    return (PyObject*)result;

  on_error:
    Py_XDECREF(args2);
    Py_XDECREF(kwargs2);
    Py_XDECREF(result);
    return NULL;
}

// non-blocking operations
#ifdef HAVE_ASYNCIO

static const char wsql_connection_run_async__doc__[] =
"Call repeatedly to continue on the non-blocking connect.  Returns a\n"
"net_async_status of either NET_ASYNC_COMPLETE or NET_ASYNC_NOT_READY.\n";

static PyObject* wsql_connection_run_async(wsql_connection *self)
{
    net_async_status status;
    int error;

    CHECK_CONNECTION(self, NULL);
    status = mysql_real_connect_nonblocking_run(&(self->connection), &error);

    if (status == NET_ASYNC_COMPLETE && error)
    {
        TRACE2("%p, %d", self, error);
        return wsql_raise_error(self);
    }

    return Py_BuildValue("(iO)", (int)status, Py_None);
}

static const char wsql_connection_file_descriptor__doc__[] =
"The file descriptor of the connection for use with non-blocking socket operations.";

static PyObject *
wsql_connection_get_file_descriptor(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong(mysql_get_file_descriptor(&(self->connection)));
}

static const char wsql_connection_async_operation__doc__[] =
"The NET_ASYNC_OP_READING or NET_ASYNC_OP_WRITING depending on if \n"
"the connection is waiting for data to be read or written.";

static PyObject* wsql_connection_get_async_operation(wsql_connection *self, void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong(self->connection.net.async_blocking_state);
}

static const char wsql_connection_query_async__doc__[] =
"Execute query in a non-blocking manner.\n"
"Returns a status.  Keep calling this function to complete the query;\n" \
"If status is NET_ASYNC_NOT_READY, the descriptor should be waited.\n" \
"Otherwise, status is NET_ASYNC_COMPLETE, and the query is complete.\n"
"The query string should be hold while query is executed";

static PyObject* wsql_connection_query_async(wsql_connection *self, PyObject *args)
{
    char* query;
    Py_ssize_t query_size;
    net_async_status status;
    int error;

    if (!PyArg_ParseTuple(args, "s#:query_async", &query, &query_size))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    status = mysql_real_query_nonblocking(&(self->connection), query, query_size, &error);

    if (status == NET_ASYNC_COMPLETE && error)
    {
        return wsql_raise_error(self);
    }

    return Py_BuildValue("(iO)", (int)status, Py_None);
}

static const char wsql_connection_next_result_async__doc__[] =
"If more query results exist, next_result() reads the next query\n" \
"results and returns the status back to application.\n" \
"After calling next_result() the state of the connection is as if\n" \
"you had called query() for the next query. This means that you can\n" \
"now call store_result(), warning_count, affected_rows, and so forth. \n" \
"Returns True if there are more results.\n";

static PyObject* wsql_connection_next_result_async(wsql_connection *self)
{
    int error;
    net_async_status status;
    CHECK_CONNECTION(self, NULL);
    status = mysql_next_result_nonblocking(&(self->connection), &error);
    if (status == NET_ASYNC_COMPLETE && error > 0)
    {
        return wsql_raise_error(self);
    }
    return Py_BuildValue("(ii)", (int)status, !error);
}

static const char wsql_connection_select_db_async__doc__[] =
"Causes the database specified by db to become the default\n" \
"(current) database on the connection specified by mysql. In subsequent\n" \
"queries, this database is the default for table references that do not\n" \
"include an explicit database specifier.\n" \
"Fails unless the connected user can be authenticated as having\n" \
"permission to use the database.\n" \
"Non-standard. (async version)\n";

static PyObject* wsql_connection_select_db_async(wsql_connection *self, PyObject *args)
{
    char *db;
    int error;
    net_async_status status;
    if (!PyArg_ParseTuple(args, "s:select_db", &db))
        return NULL;

    TRACE1("%p", self);
    CHECK_CONNECTION(self, NULL);

    status = mysql_select_db_nonblocking(&(self->connection), db, &error);
    if (status == NET_ASYNC_COMPLETE && error)
    {
        TRACE2("%p, %d", self, error);
        return wsql_raise_error(self);
    }
    return Py_BuildValue("(iO)", (int)status, Py_None);
}

#endif //HAVE_ASYNCIO


static const char wsql_connection_thread_id__doc__[] =
"Returns the thread ID of the current connection. This value\n" \
"can be used as an argument to kill() to kill the thread.\n" \
"If the connection is lost and you reconnect with ping(), the thread ID will change.\n" \
"This means you should not get the thread ID and store it for later.\n" \
"You should get it when you need it.\n" \
"Non-standard.\n";

static PyObject* wsql_connection_get_thread_id(wsql_connection *self, void* closure)
{
    unsigned long pid;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    pid = mysql_thread_id(&(self->connection));
    Py_END_ALLOW_THREADS
    return PyLong_FromLong((long)pid);
}

static const char wsql_connection_closed__doc__[] =
"Returns the True if connection has been already closed, otherwise False.\n";

static PyObject* wsql_connection_get_closed(wsql_connection *self, void* closure)
{
    return PyBool_FromLong(!(self->open));
}

static void wsql_connection_dealloc(wsql_connection *self)
{
    TRACE1("%p", self);
    if (self->open)
        Py_XDECREF(wsql_connection_close(self));
    Py_FREE(self);
}

static PyObject* wsql_connection_repr(wsql_connection *self)
{
    char buf[300];
    if (self->open)
        sprintf(buf, "<" STRINGIFY(MODULE_NAME) ".Connection open to '%.256s' at %p>", self->connection.host, self);
    else
        sprintf(buf, "<" STRINGIFY(MODULE_NAME) ".Connection closed at %p>", self);
    return PyString_FromString(buf);
}

static PyMethodDef wsql_connection_methods[] = {
    {
        "commit",
        (PyCFunction)wsql_connection_commit,
        METH_NOARGS,
        wsql_connection_commit__doc__
    },
    {
        "rollback",
        (PyCFunction)wsql_connection_rollback,
        METH_NOARGS,
        wsql_connection_rollback__doc__
    },
    {
        "next_result",
        (PyCFunction)wsql_connection_next_result,
        METH_NOARGS,
        wsql_connection_next_result__doc__
    },
#if MYSQL_VERSION_ID >= 40100
    {
        "set_server_option",
        (PyCFunction)wsql_connection_set_server_option,
        METH_VARARGS,
        wsql_connection_set_server_option__doc__
    },
#endif
#if MYSQL_VERSION_ID >= 32303
    {
        "change_user",
        (PyCFunction)wsql_connection_change_user,
        METH_VARARGS | METH_KEYWORDS,
        wsql_connection_change_user__doc__
    },
#endif
    {
        "close",
        (PyCFunction)wsql_connection_close,
        METH_NOARGS,
        wsql_connection_close__doc__
    },
    {
        "dump_debug_info",
        (PyCFunction)wsql_connection_dump_debug_info,
        METH_NOARGS,
        wsql_connection_dump_debug_info__doc__
    },
    {
        "escape",
        (PyCFunction)wsql_connection_escape_string,
        METH_VARARGS,
        wsql_connection_escape_string__doc__
    },
    {
        "quote",
        (PyCFunction)wsql_connection_string_quote,
        METH_VARARGS,
        wsql_connection_string_quote__doc__
    },
    {
        "get_result",
        (PyCFunction)wsql_connection_get_result,
        METH_VARARGS | METH_KEYWORDS,
        wsql_connection_get_result__doc__
    },
    {
        "kill",
        (PyCFunction)wsql_connection_kill,
        METH_VARARGS,
        wsql_connection_kill__doc__
    },
    {
        "ping",
        (PyCFunction)wsql_connection_ping,
        METH_VARARGS,
        wsql_connection_ping__doc__
    },
    {
        "query",
        (PyCFunction)wsql_connection_query,
        METH_VARARGS,
        wsql_connection_query__doc__
    },
    {
        "select_db",
        (PyCFunction)wsql_connection_select_db,
        METH_VARARGS,
        wsql_connection_select_db__doc__
    },
    {
        "shutdown",
        (PyCFunction)wsql_connection_shutdown,
        METH_NOARGS,
        wsql_connection_shutdown__doc__
    },
    {
        "set_sql_mode",
        (PyCFunction)wsql_connection_set_sql_mode,
        METH_VARARGS,
        wsql_connection_set_sql_mode__doc__
    },
#ifdef HAVE_ASYNCIO
    {
        "run_async",
        (PyCFunction)wsql_connection_run_async,
        METH_NOARGS,
        wsql_connection_run_async__doc__
    },
    {
        "query_async",
        (PyCFunction)wsql_connection_query_async,
        METH_VARARGS,
        METH_VARARGS,
        wsql_connection_query_async__doc__
    },
    {
        "select_db_async",
        (PyCFunction)wsql_connection_select_db_async,
        METH_VARARGS,
        wsql_connection_select_db_async__doc__
    },
    {
        "next_result_async",
        (PyCFunction)wsql_connection_next_result_async,
        METH_NOARGS,
        wsql_connection_next_result_async__doc__
    },
#endif
    {NULL} /* sentinel */
};

static PyMemberDef wsql_connection_members[] = {
    {
        "open",
        T_INT,
        offsetof(wsql_connection, open),
        READONLY,
        "True if connection is open"
    },
    {
        "server_capabilities",
        T_UINT,
        offsetof(wsql_connection, connection.server_capabilities),
        READONLY,
        "Capabilites of server; consult MySQLdb.constants.CLIENT"
    },
    {
         "port",
         T_UINT,
         offsetof(wsql_connection, connection.port),
         READONLY,
         "TCP/IP port of the server connection"
    },
    {
         "client_flag",
         T_UINT,
         offsetof(wsql_connection, connection.client_flag),
         READONLY,
         "Client flags; refer to MySQLdb.constants.CLIENT"
    },
    {
         "connected",
         T_INT,
         offsetof(wsql_connection, connected),
         READONLY,
         "if true, connection was lost and reconnect is required."
    },

    {NULL} /* Sentinel */
};


static PyGetSetDef wsql_connection_getset[]  = {
    {
        "autocommit",
        (getter)wsql_connection_get_autocommit,
        (setter)wsql_connection_set_autocommit,
        wsql_connection_autocommit__doc__,
        NULL,
    },
    {
        "closed",
        (getter)wsql_connection_get_closed,
        NULL,
        wsql_connection_closed__doc__,
        NULL
    },
    {
        "affected_rows",
        (getter)wsql_connection_get_affected_rows,
        NULL,
        wsql_connection_affected_rows__doc__,
        NULL
    },
#if MYSQL_VERSION_ID >= 40100
    {
        "sqlstate",
        (getter)wsql_connection_get_sqlstate,
        NULL,
        wsql_connection_sqlstate__doc__,
        NULL
    },
    {
        "warning_count",
        (getter)wsql_connection_get_warning_count,
        NULL,
        wsql_connection_warning_count__doc__,
        NULL
    },
#endif
    {
        "charset",
        (getter)wsql_connection_get_charset,
        (setter)wsql_connection_set_charset,
        wsql_connection_charset__doc__,
        NULL
    },
#if MYSQL_VERSION_ID >= 50010
    {
        "charset_info",
        (getter)wsql_connection_get_charset_info,
        NULL,
        wsql_connection_charset_info__doc__,
        NULL
    },
#endif
    {
        "error",
        (getter)wsql_connection_get_error,
        NULL,
        wsql_connection_error__doc__,
        NULL
    },
    {
        "errno",
        (getter)wsql_connection_get_errno,
        NULL,
        wsql_connection_errno__doc__,
        NULL
    },
    {
        "field_count",
        (getter)wsql_connection_get_field_count,
        NULL,
        wsql_connection_field_count__doc__,
        NULL
    },
    {
        "host_info",
        (getter)wsql_connection_get_host_info,
        NULL,
        wsql_connection_host_info__doc__,
        NULL
    },
    {
        "proto_info",
        (getter)wsql_connection_get_proto_info,
        NULL,
        wsql_connection_proto_info__doc__,
        NULL
    },
        {
        "server_info",
        (getter)wsql_connection_get_server_info,
        NULL,
        wsql_connection_server_info__doc__,
        NULL
    },
    {
        "info",
        (getter)wsql_connection_get_info,
        NULL,
        wsql_connection_info__doc__,
        NULL
    },
    {
        "last_insert_id",
        (getter)wsql_connection_get_insert_id,
        NULL,
        wsql_connection_insert_id__doc__,
        NULL
    },
    {
        "stat",
        (getter)wsql_connection_get_stat,
        NULL,
        wsql_connection_stat__doc__,
        NULL
    },
    {
        "thread_id",
        (getter)wsql_connection_get_thread_id,
        NULL,
        wsql_connection_thread_id__doc__,
        NULL
    },
#ifdef HAVE_ASYNCIO
    {
        "fd",
        (getter)wsql_connection_get_file_descriptor,
        NULL,
        wsql_connection_file_descriptor__doc__,
        NULL
    },
    {
        "async_operation",
        (getter)wsql_connection_get_async_operation,
        NULL,
        wsql_connection_async_operation__doc__,
        NULL
    },

#endif
    {NULL} /* Sentinel */
};

PyTypeObject wsql_connection_t = {
    PyVarObject_HEAD_INIT(NULL, 0)
    STRINGIFY(MODULE_NAME) ".Connection",   /* tp_name */
    sizeof(wsql_connection),                /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor)wsql_connection_dealloc,    /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_reserved */
    (reprfunc)wsql_connection_repr,         /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash  */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    wsql_connect__doc__,                    /* tp_doc */
    0,                                      /* tp_traverse */
    (inquiry)wsql_connection_clear,         /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    wsql_connection_methods,                /* tp_methods */
    wsql_connection_members,                /* tp_members */
    wsql_connection_getset,                 /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    (initproc)wsql_connection__init__,      /* tp_init */
    PyType_GenericAlloc,                    /* tp_alloc */
    (newfunc)_PyObject_NewVar,              /* tp_new */
    PyObject_Del                            /* tp_del */
};
