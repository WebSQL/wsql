/* -*- mode: C; indent-spaces-mode: t; c-basic-offset: 4; -*- */

#include "mysqlmod.h"

int
_mysql_connection_object__init__(
    _mysql_connection_object *self,
    PyObject *args,
    PyObject *kwargs)
{
    TRACE1("%p", self);

    MYSQL *conn = NULL;
    PyObject *ssl = NULL;
#if HAVE_OPENSSL
    char *key = NULL, *cert = NULL, *ca = NULL,
         *capath = NULL, *cipher = NULL;
#endif
    char *host = NULL, *user = NULL, *password = NULL,
         *database = NULL, *socket_name = NULL,
         *init_command=NULL, *read_default_file=NULL, *read_default_group=NULL;
    unsigned int port = 0, client_flag = 0, connect_timeout = 0;
    int nonblocking = 0, compress = -1, local_infile = -1;

    static char *kwlist[] = {
        "host", "user", "password", "database",
        "port", "socket_name", "connect_timeout", "compress",
        "init_command", "read_default_file", "read_default_group", "client_flag",
        "ssl", "local_infile", "nonblocking",
        NULL
    };

    self->open = 0;
    self->autocommit = 0;
    CHECK_SERVER(-1);

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ssssIsIisssIOii:connect",
                     kwlist,
                     &host, &user, &password, &database,
                     &port, &socket_name, &connect_timeout, &compress,
                     &init_command, &read_default_file, &read_default_group,
                     &client_flag, &ssl, &local_infile, &nonblocking))
        return -1;

    if (nonblocking) {
#ifndef HAVE_ASYNCIO
        PyErr_SetString(_mysql_not_supported_error, "client library does not have non-blocking operations support");
        return -1;
#endif
    }

#define GET_ITEM_STRING(k, t, d) \
{ t = PyMapping_GetItemString(d, #k);\
  if (t) { k = PyString_AsString(t); Py_DECREF(t);} \
  PyErr_Clear(); \
}

    if (ssl) {
#if HAVE_OPENSSL
        PyObject *value = NULL;
        GET_ITEM_STRING(ca, value, ssl);
        GET_ITEM_STRING(capath, value, ssl);
        GET_ITEM_STRING(cert, value, ssl);
        GET_ITEM_STRING(key, value, ssl);
        GET_ITEM_STRING(cipher, value, ssl);
#else
        PyErr_SetString(_mysql_not_supported_error, "client library does not have SSL support");
        return -1;
#endif
    }
#undef GET_ITEM_STRING
    Py_BEGIN_ALLOW_THREADS ;
    conn = mysql_init(&(self->connection));

    if (compress != -1) {
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

    if (nonblocking) {
#ifdef HAVE_ASYNCIO
        if (!mysql_real_connect_nonblocking_init(&(self->connection),
                host, user, password, database, port, socket_name, client_flag))
            conn = NULL;
#endif
    } else {
        conn = mysql_real_connect(&(self->connection),
            host, user, password, database, port, socket_name, client_flag);
    }

    self->autocommit = self->connection.server_capabilities & CLIENT_TRANSACTIONS ? 1 : 0;
    Py_END_ALLOW_THREADS ;

    if (conn == NULL) {
        _mysql_exception(self);
        return -1;
    }

    self->open = 1;
    return 0;
}

char _mysql_connect__doc__[] =
"Returns a MYSQL connection object. Exclusive use of\n" \
"keyword parameters strongly recommended. Consult the\n" \
"MySQL C API documentation for more details.\n" \
"\n " \
"host\n" \
"  string, host to connect\n" \
"\n" \
"user\n" \
"  string, user to connect as\n" \
"\n" \
"password\n" \
"  string, password to use\n" \
"\n" \
"db\n" \
"  string, database to use\n" \
"\n" \
"port\n" \
"  integer, TCP/IP port to connect to\n" \
"\n" \
"socket_name\n" \
"  string, location of unix-socket or windows named pipe\n" \
"\n" \
"connect_timeout\n" \
"  number of seconds to wait before the connection\n" \
"  attempt fails.\n" \
"\n" \
"compress\n" \
"  if set, gzip compression is enabled\n" \
"\n" \
"init_command\n" \
"  command which is run once the connection is created\n" \
"\n" \
"read_default_file\n" \
"  see the MySQL documentation for mysql_options()\n" \
"\n" \
"read_default_group\n" \
"  see the MySQL documentation for mysql_options()\n" \
"\n" \
"client_flag\n" \
"  client flags from MySQLdb.constants.CLIENT\n" \
"\n" \
"load_infile\n" \
"  int, non-zero enables LOAD LOCAL INFILE, zero disables\n" \
"\n";

PyObject *
_mysql_connect(
    PyObject *self,
    PyObject *args,
    PyObject *kwargs)
{
    _mysql_connection_object *c = NULL;
    if (!(c = Py_ALLOC(_mysql_connection_object, _mysql_connection_object_t)))
        return NULL;
    if (_mysql_connection_object__init__(c, args, kwargs) < 0) {
        Py_DECREF(c);
        c = NULL;
    }
    return (PyObject *) c;
}

static int
_mysql_connection_object_clear(
    _mysql_connection_object *self)
{
    TRACE1("%p", self);
    return 0;
}


char _mysql_connection_object_escape_string__doc__[] =
"escape_string(s) -- quote any SQL-interpreted characters in string s.\n " \
"If you want quotes around your value, use string_literal(s) instead.\n";

static PyObject *
_mysql_connection_object_escape_string(
    _mysql_connection_object *self,
    PyObject *args)
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
    if (output_size != input_size << 1 && _PyBytes_Resize(&output, output_size) < 0) {
        return NULL;
    }
    return output;
}

char _mysql_connection_object_string_quote__doc__[] =
"quote(s) -- converts string s into a SQL string literal.\n" \
"This means, any special SQL characters are escaped, and it is enclosed\n" \
"within single quotes. In other words, it performs:\n" \
"\n" \
"\"'%s'\" % escape(s)\n";

static PyObject *
_mysql_connection_object_string_quote(
    _mysql_connection_object *self,
    PyObject *args)
{
    PyObject *output;
    Py_ssize_t input_size;
    unsigned long output_size;
    char *input_string, *output_string;
    if (!PyArg_ParseTuple(args, "s#:quote", &input_string, &input_size))
        return NULL;

    if (!(output = PyBytes_FromStringAndSize(NULL, (input_size << 1) + 2))) {
        return PyErr_NoMemory();
    }

    output_string = PyBytes_AS_STRING(output);
    output_size = mysql_real_escape_string(&(self->connection), output_string + 1, input_string, input_size);
    output_string[0] = output_string[output_size + 1] = '\'';
    if (output_size != input_size << 1 && _PyBytes_Resize(&output, output_size + 2) < 0) {
        return NULL;
    }
    return output;
}

static char _mysql_connection_object_close__doc__[] =
"Close the connection. No further activity possible.";

static PyObject *
_mysql_connection_object_close(
    _mysql_connection_object *self)
{
    if (self->open) {
        Py_BEGIN_ALLOW_THREADS
        mysql_close(&(self->connection));
        Py_END_ALLOW_THREADS
        self->open = 0;
    } else {
        PyErr_SetString(_mysql_programming_error, "closing a closed connection");
        return NULL;
    }
    _mysql_connection_object_clear(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_affected_rows__doc__ [] =
"Return number of rows affected by the last query.\n" \
"Non-standard. Use Cursor.rowcount.\n";

static PyObject *
_mysql_connection_object_affected_rows(
    _mysql_connection_object *self,
     void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromUnsignedLongLong(mysql_affected_rows(&(self->connection)));
}

static char _mysql_connection_object_dump_debug_info__doc__[] =
"Instructs the server to write some debug information to the\n" \
"log. The connected user must have the process privilege for\n" \
"this to work. Non-standard.\n";

static PyObject *
_mysql_connection_object_dump_debug_info(
    _mysql_connection_object *self)
{
    int error;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    error = mysql_dump_debug_info(&(self->connection));
    Py_END_ALLOW_THREADS
    if (error)
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_autocommit__doc__[] =
"the autocommit mode. True means enable; False means disable.\n";

static PyObject *
_mysql_connection_object_get_autocommit(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyBool_FromLong(self->autocommit);
}

static int
_mysql_connection_object_set_autocommit(
    _mysql_connection_object *self,
    PyObject *value,
    void* closure)
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
    if (error) {
        _mysql_exception(self);
        return -1;
    }
    self->autocommit = flag ? 1 : 0;
    return 0;
}

static char _mysql_connection_object_set_sql_mode__doc__[] =
"Set the connection sql_mode. See MySQL documentation for legal values.\n"\
"Non-standard. It is better to set this when creating the connection\n"\
"using the sql_mode parameter.\n";

static PyObject *
_mysql_connection_object_set_sql_mode(
    _mysql_connection_object *self,
    PyObject *args)
{
    int error = 0;
    char* sql_mode;
    if (!PyArg_ParseTuple(args, "s:set_sql_mode", &sql_mode))
        return NULL;

    CHECK_CONNECTION(self, NULL);

    Py_BEGIN_ALLOW_THREADS
    if (mysql_get_server_version(&(self->connection)) > 40100) {
        char query[256];
        snprintf(query, 256, "SET SESSION sql_mode='%s'", sql_mode);
        error = mysql_query(&(self->connection), query);
    }
    Py_END_ALLOW_THREADS
    if (error)
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_commit__doc__[] =
"Commits the current transaction\n";

static PyObject *
_mysql_connection_object_commit(
    _mysql_connection_object *self)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_rollback__doc__[] =
"Rolls backs the current transaction\n\
";
static PyObject *
_mysql_connection_object_rollback(
    _mysql_connection_object *self)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_next_result__doc__[] =
"If more query results exist, next_result() reads the next query\n" \
"results and returns the status back to application.\n" \
"After calling next_result() the state of the connection is as if\n" \
"you had called query() for the next query. This means that you can\n" \
"now call store_result(), warning_count, affected_rows, and so forth. \n" \
"Returns True if there are more results.\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_next_result(
    _mysql_connection_object *self)
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
        return _mysql_exception(self);
    return PyBool_FromLong(!error);
}

#if MYSQL_VERSION_ID >= 40100

static char _mysql_connection_object_set_server_option__doc__[] =
"set_server_option(option) -- Enables or disables an option for the connection.\n" \
"\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_set_server_option(
    _mysql_connection_object *self,
    PyObject *args)
{
    int error, flags=0;
    if (!PyArg_ParseTuple(args, "i", &flags))
        return NULL;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    error = mysql_set_server_option(&(self->connection), flags);
    Py_END_ALLOW_THREADS
    if (error)
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_sqlstate__doc__[] =
"Returns a string containing the SQLSTATE error code\n" \
"for the last error. The error code consists of five characters.\n" \
"'00000' means \"no error.\" The values are specified by ANSI SQL\n" \
"and ODBC. For a list of possible values, see section 23\n" \
"Error Handling in MySQL in the MySQL Manual.\n" \
"\n" \
"Note that not all MySQL errors are yet mapped to SQLSTATE's.\n" \
"The value 'HY000' (general error) is used for unmapped errors.\n" \
"\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_sqlstate(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyString_FromString(mysql_sqlstate(&(self->connection)));
}

static char _mysql_connection_object_warning_count__doc__[] =
"Returns the number of warnings generated during execution\n" \
"of the previous SQL statement.\n" \
"\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_warning_count(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong(mysql_warning_count(&(self->connection)));
}

#endif

static char _mysql_connection_object_errno__doc__[] =
"Returns the error code for the most recently invoked API function\n "\
"that can succeed or fail. A return value of zero means that no error occurred.\n";

static PyObject *
_mysql_connection_object_errno(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong((long)mysql_errno(&(self->connection)));
}

static char _mysql_connection_object_error__doc__[] =
"Returns the error message for the most recently invoked API function\n\
that can succeed or fail. An empty string ("") is returned if no error\n\
occurred.\n\
";

static PyObject *
_mysql_connection_object_error(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyString_FromString(mysql_error(&(self->connection)));
}

#if MYSQL_VERSION_ID >= 32303

static char _mysql_connection_object_change_user__doc__[] =
"Changes the user and causes the database specified by db to\n" \
"become the default (current) database on the connection\n" \
"specified by mysql. In subsequent queries, this database is\n" \
"the default for table references that do not include an\n" \
"explicit database specifier.\n" \
"\n" \
"This function was introduced in MySQL Version 3.23.3.\n" \
"\n" \
"Fails unless the connected user can be authenticated or if he\n" \
"doesn't have permission to use the database. In this case the\n" \
"user and database are not changed.\n" \
"\n" \
"The db parameter may be set to None if you don't want to have\n" \
"a default database.\n";

static PyObject *
_mysql_connection_object_change_user(
    _mysql_connection_object *self,
    PyObject *args,
    PyObject *kwargs)
{
    static char *kwlist[] = { "user", "passwd", "db", NULL } ;
    char *user, *pwd=NULL, *db=NULL;
    int error;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss:change_user", kwlist, &user, &pwd, &db))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    error = mysql_change_user(&(self->connection), user, pwd, db);
    Py_END_ALLOW_THREADS
    if (error)
        return _mysql_exception(self);
    Py_RETURN_NONE;
}
#endif

static char _mysql_connection_object_charset__doc__[] =
"the default character set for the current connection.\n " \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_get_charset(
    _mysql_connection_object *self,
    void* closure)
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

static int
_mysql_connection_object_set_charset(
    _mysql_connection_object *self,
    PyObject *arg,
    void* closure)
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
    if (error) {
        _mysql_exception(self);
        return -1;
    }
    return 0;
}

#if MYSQL_VERSION_ID >= 50010
static char _mysql_connection_object_charset_info__doc__[] =
"A dict with information about the current character set:\n" \
"\n" \
"collation\n" \
"    collation name\n" \
"name\n" \
"    character set name\n" \
"comment\n" \
"    comment or descriptive name\n" \
"dir\n" \
"    character set directory\n" \
"mbminlen\n" \
"    min. length for multibyte string\n" \
"mbmaxlen\n" \
"    max. length for multibyte string\n" \
"\n" \
"Not all keys may be present, particularly dir.\n" \
"\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_get_charset_info(
    _mysql_connection_object *self,
    void* closure)
{
    PyObject *result;
    MY_CHARSET_INFO cs;

    CHECK_CONNECTION(self, NULL);

    mysql_get_character_set_info(&(self->connection), &cs);
    if (!(result = PyDict_New()))
        return NULL;

    if (cs.csname)
        PyDict_SetItemString(result, "name", PyString_FromString(cs.csname));
    if (cs.name)
        PyDict_SetItemString(result, "collation", PyString_FromString(cs.name));
    if (cs.comment)
        PyDict_SetItemString(result, "comment", PyString_FromString(cs.comment));
    if (cs.dir)
        PyDict_SetItemString(result, "dir", PyString_FromString(cs.dir));

    PyDict_SetItemString(result, "mbminlen", PyLong_FromLong(cs.mbminlen));
    PyDict_SetItemString(result, "mbmaxlen", PyLong_FromLong(cs.mbmaxlen));
    return result;
}
#endif

static char _mysql_connection_object_get_host_info__doc__[] =
"Returns a string that represents the MySQL client library version.\n"
"Non-standard.\n";

static PyObject *
_mysql_connection_object_get_host_info(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyString_FromString(mysql_get_host_info(&(self->connection)));
}

static char _mysql_connection_object_get_proto_info__doc__[] =
"Returns an unsigned integer representing the protocol version\n" \
"used by the current connection. Non-standard.\n";

static PyObject *
_mysql_connection_object_get_proto_info(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong((long)mysql_get_proto_info(&(self->connection)));
}

static char _mysql_connection_object_get_server_info__doc__[] =
"a string that represents the server version number.\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_get_server_info(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    const char* info = mysql_get_server_info(&(self->connection));
    if (!info) {
        PyErr_SetString(_mysql_programming_error, "connection is not ready");
        return NULL;
    }
    return PyString_FromString(info);
}

static char _mysql_connection_object_info__doc__[] =
"Retrieves a string providing information about the most\n" \
"recently executed query. Non-standard. Use messages or\n" \
"Cursor.messages.\n";

static PyObject *
_mysql_connection_object_info(
    _mysql_connection_object *self,
    void* closure)
{
    const char *s;
    CHECK_CONNECTION(self, NULL);
    s = mysql_info(&(self->connection));
    if (s)
        return PyString_FromString(s);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_insert_id__doc__[] =
"Returns the ID generated for an AUTO_INCREMENT column by the previous\n" \
"query. Use this function after you have performed an INSERT query into a\n" \
"table that contains an AUTO_INCREMENT field.\n" \
"\n" \
"Note that this returns 0 if the previous query does not\n" \
"generate an AUTO_INCREMENT value. If you need to save the value for\n" \
"later, be sure to call this immediately after the query\n" \
"that generates the value.\n" \
"\n" \
"The ID is updated after INSERT and UPDATE statements that generate\n" \
"an AUTO_INCREMENT value or that set a column value to\n" \
"LAST_INSERT_ID(expr). See section 6.3.5.2 Miscellaneous Functions\n" \
"in the MySQL documentation.\n" \
"\n" \
"Also note that the value of the SQL LAST_INSERT_ID() function always\n" \
"contains the most recently generated AUTO_INCREMENT value, and is not\n" \
"reset between queries because the value of that function is maintained\n" \
"in the server.\n";

static PyObject *
_mysql_connection_object_insert_id(
    _mysql_connection_object *self,
    void* closure)
{
    my_ulonglong r;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    r = mysql_insert_id(&(self->connection));
    Py_END_ALLOW_THREADS
    return PyLong_FromUnsignedLongLong(r);
}

static char _mysql_connection_object_kill__doc__[] =
"Asks the server to kill the thread specified by pid.\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_kill(
    _mysql_connection_object *self,
    PyObject *args)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_field_count__doc__[] =
"Returns the number of columns for the most recent query on the\n" \
"connection. Non-standard. Will probably give you bogus results\n" \
"on most cursor classes. Use Cursor.rowcount.\n";

static PyObject *
_mysql_connection_object_field_count(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
#if MYSQL_VERSION_ID < 32224
    return PyLong_FromLong((long)mysql_num_fields(&(self->connection)));
#else
    return PyLong_FromLong((long)mysql_field_count(&(self->connection)));
#endif
}

static char _mysql_connection_object_ping__doc__[] =
"Checks whether or not the connection to the server is\n" \
"working. If it has gone down, an automatic reconnection is\n" \
"attempted.\n" \
"\n" \
"This function can be used by clients that remain idle for a\n" \
"long while, to check whether or not the server has closed the\n" \
"connection and reconnect if necessary.\n" \
"\n" \
"New in 1.2.2: Accepts an optional reconnect parameter. If True,\n" \
"then the client will attempt reconnection. Note that this setting\n" \
"is persistent. By default, this is on in MySQL<5.0.3, and off\n" \
"thereafter.\n" \
"\n" \
"Non-standard. You should assume that ping() performs an\n" \
"implicit rollback; use only when starting a new transaction.\n" \
"You have been warned.\n";

static PyObject *
_mysql_connection_object_ping(
    _mysql_connection_object *self,
    PyObject *args)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_query__doc__[] =
"Execute a query. store_result() or use_result() will get the\n" \
"result set, if any. Non-standard. Use cursor() to create a cursor,\n" \
"then cursor.execute().\n";

static PyObject *
_mysql_connection_object_query(
    _mysql_connection_object *self,
    PyObject *args)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}


static char _mysql_connection_object_select_db__doc__[] =
"Causes the database specified by db to become the default\n" \
"(current) database on the connection specified by mysql. In subsequent\n" \
"queries, this database is the default for table references that do not\n" \
"include an explicit database specifier.\n" \
"Fails unless the connected user can be authenticated as having\n" \
"permission to use the database.\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_select_db(
    _mysql_connection_object *self,
    PyObject *args)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_shutdown__doc__[] =
"Asks the database server to shut down. The connected user must\n" \
"have shutdown privileges. Non-standard.\n";

static PyObject *
_mysql_connection_object_shutdown(
    _mysql_connection_object *self)
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
        return _mysql_exception(self);
    Py_RETURN_NONE;
}

static char _mysql_connection_object_stat__doc__[] =
"a character string containing information similar to\n" \
"that provided by the mysqladmin status command. This includes\n" \
"uptime in seconds and the number of running threads,\n" \
"questions, reloads, and open tables. Non-standard.\n";

static PyObject *
_mysql_connection_object_stat(
    _mysql_connection_object *self,
    void* closure)
{
    const char *s;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    s = mysql_stat(&(self->connection));
    Py_END_ALLOW_THREADS
    if (!s)
        return _mysql_exception(self);
    return PyString_FromString(s);
}

static char _mysql_connection_object_get_result__doc__[] =
"Returns a result object. If use is True, mysql_use_result()\n" \
"is used; otherwise mysql_store_result() is used (the default).\n";

static PyObject *
_mysql_connection_object_get_result(
    _mysql_connection_object *self,
    PyObject *args,
    PyObject *kwargs)
{
    static char *kwlist[] = {"use", NULL};
    PyObject *args2 = NULL, *kwargs2 = NULL;
    _mysql_result_object *result = NULL;
    int use = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|i:get_result", kwlist, &use))
        return NULL;

    CHECK_CONNECTION(self, NULL);

    if (!(args2 = Py_BuildValue("(Oi)", self, use)))
        goto on_error;

    if (!(kwargs2 = PyDict_New()))
        goto on_error;

    if (!(result = Py_ALLOC(_mysql_result_object, _mysql_result_object_t)))
        goto on_error;

    if (_mysql_result_object__init__(result, args2, kwargs2) < 0)
        goto on_error;

    Py_DECREF(args2);
    Py_DECREF(kwargs2);

    if (!(result->result)) {
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

static char _mysql_connection_object_run_async__doc__[] =
  "Call repeatedly to continue on the non-blocking connect.  Returns a\n"
  "net_async_status of either NET_ASYNC_COMPLETE or NET_ASYNC_NOT_READY.\n";

static PyObject *
_mysql_connection_object_run_async(
    _mysql_connection_object *self)
{
    net_async_status status;
    int error;
    CHECK_CONNECTION(self, NULL);
    status = mysql_real_connect_nonblocking_run(&(self->connection), &error);

    if (status == NET_ASYNC_COMPLETE && error) {
        TRACE2("%p, %d", self, error);
        return _mysql_exception(self);
    }

    return Py_BuildValue("(iO)", (int)status, Py_None);
}

static char _mysql_connection_object_get_file_descriptor__doc__[] =
  "The file descriptor of the connection for use with non-blocking socket operations.";

static PyObject *
_mysql_connection_object_get_file_descriptor(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong(mysql_get_file_descriptor(&(self->connection)));
}


static char _mysql_connection_object_get_async_operation__doc__[] =
  "The NET_ASYNC_OP_READING or NET_ASYNC_OP_WRITING depending on if the\n"
  "connection is waiting for data to be read or written.";

static PyObject *
_mysql_connection_object_get_async_operation(
    _mysql_connection_object *self,
    void* closure)
{
    CHECK_CONNECTION(self, NULL);
    return PyLong_FromLong(self->connection.net.async_blocking_state);
}

static char _mysql_connection_object_query_async__doc__[] =
  "Execute query in a non-blocking manner.\n"
  "Returns a status.  Keep calling this function to\n"
  "complete the query; If status is NET_ASYNC_NOT_READY,\n"
  "the descriptor should be waited.  Otherwise, status\n"
  "is NET_ASYNC_COMPLETE, and the query is complete.\n"
  "the query string should be hold while query is executed";

static PyObject *
_mysql_connection_object_query_async(
    _mysql_connection_object *self,
    PyObject *args)
{
    char* query;
    Py_ssize_t query_size;
    net_async_status status;
    int error;

    if (!PyArg_ParseTuple(args, "s#:query_async", &query, &query_size))
        return NULL;

    CHECK_CONNECTION(self, NULL);
    status = mysql_real_query_nonblocking(&(self->connection), query, query_size, &error);

    if (status == NET_ASYNC_COMPLETE && error) {
        TRACE2("%p, %d", self, error);
        return _mysql_exception(self);
    }
    return Py_BuildValue("(iO)", (int)status, Py_None);
}

static char _mysql_connection_object_next_result_async__doc__[] =
"If more query results exist, next_result() reads the next query\n" \
"results and returns the status back to application.\n" \
"After calling next_result() the state of the connection is as if\n" \
"you had called query() for the next query. This means that you can\n" \
"now call store_result(), warning_count, affected_rows, and so forth. \n" \
"Returns True if there are more results.\n";

static PyObject *
_mysql_connection_object_next_result_async(
    _mysql_connection_object *self)
{
    int error;
    net_async_status status;
    CHECK_CONNECTION(self, NULL);
    status = mysql_next_result_nonblocking(&(self->connection), &error);
    if (status == NET_ASYNC_COMPLETE && error > 0) {
        TRACE2("%p, %d", self, error);
        return _mysql_exception(self);
    }
    return Py_BuildValue("(iO)", (int)status, PyBool_FromLong(!error));
}

static char _mysql_connection_object_select_db_async__doc__[] =
"Causes the database specified by db to become the default\n" \
"(current) database on the connection specified by mysql. In subsequent\n" \
"queries, this database is the default for table references that do not\n" \
"include an explicit database specifier.\n" \
"Fails unless the connected user can be authenticated as having\n" \
"permission to use the database.\n" \
"Non-standard. (async version)\n";

static PyObject *
_mysql_connection_object_select_db_async(
    _mysql_connection_object *self,
    PyObject *args)
{
    char *db;
    my_bool error;
    net_async_status status;
    if (!PyArg_ParseTuple(args, "s:select_db", &db))
        return NULL;
    CHECK_CONNECTION(self, NULL);
    status = mysql_select_db_nonblocking(&(self->connection), db, &error);
    if (status == NET_ASYNC_COMPLETE && error) {
        TRACE2("%p, %d", self, error);
        return _mysql_exception(self);
    }
    return Py_BuildValue("(iO)", (int)status, Py_None);
}

#endif //HAVE_ASYNCIO


static char _mysql_connection_object_thread_id__doc__[] =
"Returns the thread ID of the current connection. This value\n" \
"can be used as an argument to kill() to kill the thread.\n" \
"\n" \
"If the connection is lost and you reconnect with ping(), the\n" \
"thread ID will change. This means you should not get the\n" \
"thread ID and store it for later. You should get it when you\n" \
"need it.\n" \
"Non-standard.\n";

static PyObject *
_mysql_connection_object_thread_id(
    _mysql_connection_object *self,
    void* closure)
{
    unsigned long pid;
    CHECK_CONNECTION(self, NULL);
    Py_BEGIN_ALLOW_THREADS
    pid = mysql_thread_id(&(self->connection));
    Py_END_ALLOW_THREADS
    return PyLong_FromLong((long)pid);
}


static char _mysql_connection_object_closed__doc__[] =
"Returns the True if connection has been already closed, otherwise False.\n";

static PyObject *
_mysql_connection_object_closed(
    _mysql_connection_object *self,
    void* closure)
{
    return PyBool_FromLong(!(self->open));
}


static void
_mysql_connection_object_dealloc(
    _mysql_connection_object *self)
{
    TRACE1("%p", self);
    if (self->open)
        Py_XDECREF(_mysql_connection_object_close(self));
    Py_FREE(self);
}

static PyObject *
_mysql_connection_object_repr(
    _mysql_connection_object *self)
{
    char buf[300];
    if (self->open)
        sprintf(buf, "<_mysql.Connection open to '%.256s' at %p>", self->connection.host, self);
    else
        sprintf(buf, "<_mysql.Connection closed at %p>", self);
    return PyString_FromString(buf);
}

static PyMethodDef _mysql_connection_object_methods[] = {
    {
        "commit",
        (PyCFunction)_mysql_connection_object_commit,
        METH_NOARGS,
        _mysql_connection_object_commit__doc__
    },
    {
        "rollback",
        (PyCFunction)_mysql_connection_object_rollback,
        METH_NOARGS,
        _mysql_connection_object_rollback__doc__
    },
    {
        "next_result",
        (PyCFunction)_mysql_connection_object_next_result,
        METH_NOARGS,
        _mysql_connection_object_next_result__doc__
    },
#if MYSQL_VERSION_ID >= 40100
    {
        "set_server_option",
        (PyCFunction)_mysql_connection_object_set_server_option,
        METH_VARARGS,
        _mysql_connection_object_set_server_option__doc__
    },
#endif
#if MYSQL_VERSION_ID >= 32303
    {
        "change_user",
        (PyCFunction)_mysql_connection_object_change_user,
        METH_VARARGS | METH_KEYWORDS,
        _mysql_connection_object_change_user__doc__
    },
#endif
    {
        "close",
        (PyCFunction)_mysql_connection_object_close,
        METH_NOARGS,
        _mysql_connection_object_close__doc__
    },
    {
        "dump_debug_info",
        (PyCFunction)_mysql_connection_object_dump_debug_info,
        METH_NOARGS,
        _mysql_connection_object_dump_debug_info__doc__
    },
    {
        "escape",
        (PyCFunction)_mysql_connection_object_escape_string,
        METH_VARARGS,
        _mysql_connection_object_escape_string__doc__
    },
    {
        "quote",
        (PyCFunction)_mysql_connection_object_string_quote,
        METH_VARARGS,
        _mysql_connection_object_string_quote__doc__
    },
    {
        "get_result",
        (PyCFunction)_mysql_connection_object_get_result,
        METH_VARARGS | METH_KEYWORDS,
        _mysql_connection_object_get_result__doc__
    },
    {
        "kill",
        (PyCFunction)_mysql_connection_object_kill,
        METH_VARARGS,
        _mysql_connection_object_kill__doc__
    },
    {
        "ping",
        (PyCFunction)_mysql_connection_object_ping,
        METH_VARARGS,
        _mysql_connection_object_ping__doc__
    },
    {
        "query",
        (PyCFunction)_mysql_connection_object_query,
        METH_VARARGS,
        _mysql_connection_object_query__doc__
    },
    {
        "select_db",
        (PyCFunction)_mysql_connection_object_select_db,
        METH_VARARGS,
        _mysql_connection_object_select_db__doc__
    },
    {
        "shutdown",
        (PyCFunction)_mysql_connection_object_shutdown,
        METH_NOARGS,
        _mysql_connection_object_shutdown__doc__
    },
    {
        "set_sql_mode",
        (PyCFunction)_mysql_connection_object_set_sql_mode,
        METH_VARARGS,
        _mysql_connection_object_set_sql_mode__doc__
    },
#ifdef HAVE_ASYNCIO
    {
        "run_async",
        (PyCFunction)_mysql_connection_object_run_async,
        METH_NOARGS,
        _mysql_connection_object_run_async__doc__
    },
    {
        "query_async",
        (PyCFunction)_mysql_connection_object_query_async,
        METH_VARARGS,
        _mysql_connection_object_query_async__doc__
    },
    {
        "select_db_async",
        (PyCFunction)_mysql_connection_object_select_db_async,
        METH_VARARGS,
        _mysql_connection_object_select_db_async__doc__
    },
    {
        "next_result_async",
        (PyCFunction)_mysql_connection_object_next_result_async,
        METH_NOARGS,
        _mysql_connection_object_next_result_async__doc__
    },
#endif
    {NULL} /* sentinel */
};

static struct PyMemberDef _mysql_connection_object_members[] = {
    {
        "open",
        T_INT,
        offsetof(_mysql_connection_object, open),
        READONLY,
        "True if connection is open"
    },
    {
        "server_capabilities",
        T_UINT,
        offsetof(_mysql_connection_object, connection.server_capabilities),
        READONLY,
        "Capabilites of server; consult MySQLdb.constants.CLIENT"
    },
    {
         "port",
         T_UINT,
         offsetof(_mysql_connection_object, connection.port),
         READONLY,
         "TCP/IP port of the server connection"
    },
    {
         "client_flag",
         T_UINT,
         READONLY,
         offsetof(_mysql_connection_object, connection.client_flag),
         "Client flags; refer to MySQLdb.constants.CLIENT"
    },
    {NULL} /* Sentinel */
};


static struct PyGetSetDef _mysql_connection_object_getset[]  = {
    {
        "autocommit",
        (getter)_mysql_connection_object_get_autocommit,
        (setter)_mysql_connection_object_set_autocommit,
        _mysql_connection_object_autocommit__doc__,
        NULL,
    },
    {
        "closed",
        (getter)_mysql_connection_object_closed,
        NULL,
        _mysql_connection_object_closed__doc__,
        NULL
    },
    {
        "affected_rows",
        (getter)_mysql_connection_object_affected_rows,
        NULL,
        _mysql_connection_object_affected_rows__doc__,
        NULL
    },
#if MYSQL_VERSION_ID >= 40100
    {
        "sqlstate",
        (getter)_mysql_connection_object_sqlstate,
        NULL,
        _mysql_connection_object_sqlstate__doc__,
        NULL
    },
    {
        "warning_count",
        (getter)_mysql_connection_object_warning_count,
        NULL,
        _mysql_connection_object_warning_count__doc__,
        NULL
    },
#endif
    {
        "charset",
        (getter)_mysql_connection_object_get_charset,
        (setter)_mysql_connection_object_set_charset,
        _mysql_connection_object_charset__doc__,
        NULL
    },
#if MYSQL_VERSION_ID >= 50010
    {
        "charset_info",
        (getter)_mysql_connection_object_get_charset_info,
        NULL,
        _mysql_connection_object_charset_info__doc__,
        NULL
    },
#endif
    {
        "error",
        (getter)_mysql_connection_object_error,
        NULL,
        _mysql_connection_object_error__doc__,
        NULL
    },
    {
        "errno",
        (getter)_mysql_connection_object_errno,
        NULL,
        _mysql_connection_object_errno__doc__,
        NULL
    },
    {
        "field_count",
        (getter)_mysql_connection_object_field_count,
        NULL,
        _mysql_connection_object_field_count__doc__,
        NULL
    },
    {
        "host_info",
        (getter)_mysql_connection_object_get_host_info,
        NULL,
        _mysql_connection_object_get_host_info__doc__,
        NULL
    },
    {
        "proto_info",
        (getter)_mysql_connection_object_get_proto_info,
        NULL,
        _mysql_connection_object_get_proto_info__doc__,
        NULL
    },
        {
        "server_info",
        (getter)_mysql_connection_object_get_server_info,
        NULL,
        _mysql_connection_object_get_server_info__doc__,
        NULL
    },
    {
        "info",
        (getter)_mysql_connection_object_info,
        NULL,
        _mysql_connection_object_info__doc__,
        NULL
    },
    {
        "last_insert_id",
        (getter)_mysql_connection_object_insert_id,
        NULL,
        _mysql_connection_object_insert_id__doc__,
        NULL
    },
    {
        "stat",
        (getter)_mysql_connection_object_stat,
        NULL,
        _mysql_connection_object_stat__doc__,
        NULL
    },
    {
        "thread_id",
        (getter)_mysql_connection_object_thread_id,
        NULL,
        _mysql_connection_object_thread_id__doc__,
        NULL
    },
#ifdef HAVE_ASYNCIO
    {
        "fd",
        (getter)_mysql_connection_object_get_file_descriptor,
        NULL,
        _mysql_connection_object_get_file_descriptor__doc__,
        NULL
    },
    {
        "async_operation",
        (getter)_mysql_connection_object_get_async_operation,
        NULL,
        _mysql_connection_object_get_async_operation__doc__,
        NULL
    },

#endif
    {NULL} /* Sentinel */
};

PyTypeObject _mysql_connection_object_t = {
    PyVarObject_HEAD_INIT(NULL, 0)
    STRINGIFY(MODULE_NAME) ".Connection",                          /* tp_name */
    sizeof(_mysql_connection_object),                              /* tp_basicsize */
    0,                                                             /* tp_itemsize */
    (destructor)_mysql_connection_object_dealloc,                  /* tp_dealloc */
    0,                                                             /* tp_print */
    0,                                                             /* tp_getattr */
    0,                                                             /* tp_setattr */
    0,                                                             /* tp_reserved */
    (reprfunc)_mysql_connection_object_repr,                       /* tp_repr */
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
    _mysql_connect__doc__,                                         /* tp_doc */
    0,                                                             /* tp_traverse */
    (inquiry)_mysql_connection_object_clear,                       /* tp_clear */
    0,                                                             /* tp_richcompare */
    0,                                                             /* tp_weaklistoffset */
    0,                                                             /* tp_iter */
    0,                                                             /* tp_iternext */
    _mysql_connection_object_methods,                              /* tp_methods */
    _mysql_connection_object_members,                              /* tp_members */
    _mysql_connection_object_getset,                               /* tp_getset */
    0,                                                             /* tp_base */
    0,                                                             /* tp_dict */
    0,                                                             /* tp_descr_get */
    0,                                                             /* tp_descr_set */
    0,                                                             /* tp_dictoffset */
    (initproc)_mysql_connection_object__init__,                    /* tp_init */
    PyType_GenericAlloc,                                           /* tp_alloc */
    (newfunc)_PyObject_NewVar,                                     /* tp_new */
    PyObject_Del
};
