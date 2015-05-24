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

#include "module.h"

const char wsql_constants__doc__[] =
"Various constants used by the MySQL client/server protocol.\n";

#ifdef PY3K
PyModuleDef wsql_constants_module =
{
    PyModuleDef_HEAD_INIT,
    STRINGIFY(MODULE_NAME) ".constants", /* name of module */
    wsql_constants__doc__,               /* module documentation, may be NULL */
    -1,                                  /* size of per-interpreter state of the module,
                                            or -1 if the module keeps state in global variables. */
    NULL, NULL, NULL, NULL, NULL
};
#endif  // PY3K

extern int wsql_constants_add_errors(PyObject* module);

int wsql_constants_init(PyObject* module)
{
    PyObject *constants = NULL;
#ifdef PY3K
    constants = PyModule_Create(&wsql_constants_module);
#else
    constants = Py_InitModule3(STRINGIFY(MODULE_NAME) ".constants", NULL, wsql_constants__doc__);
#endif

    if (!constants) return -1;

    if (PyModule_AddStringConstant(constants, "NULL", "NULL") < 0) goto error;

    if (wsql_constants_add_errors(constants) < 0) goto error;

#ifdef HAVE_ASYNCIO
    if (PyModule_AddIntConstant(constants, "NET_ASYNC_COMPLETE", NET_ASYNC_COMPLETE) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "NET_ASYNC_NOT_READY", NET_ASYNC_NOT_READY) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "NET_ASYNC_CONNECT", NET_NONBLOCKING_CONNECT) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "NET_ASYNC_READ", NET_NONBLOCKING_READ) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "NET_ASYNC_WRITE", NET_NONBLOCKING_WRITE) < 0) goto error;
#endif

// CLIENT_*
#ifdef CLIENT_LONG_PASSWORD
    if (PyModule_AddIntMacro(constants, CLIENT_LONG_PASSWORD) < 0) goto error;
#endif
#ifdef CLIENT_FOUND_ROWS
    if (PyModule_AddIntMacro(constants, CLIENT_FOUND_ROWS) < 0) goto error;
#endif
#ifdef CLIENT_LONG_FLAG
    if (PyModule_AddIntMacro(constants, CLIENT_LONG_FLAG) < 0) goto error;
#endif
#ifdef CLIENT_CONNECT_WITH_DB
    if (PyModule_AddIntMacro(constants, CLIENT_CONNECT_WITH_DB) < 0) goto error;
#endif
#ifdef CLIENT_NO_SCHEMA
    if (PyModule_AddIntMacro(constants, CLIENT_NO_SCHEMA) < 0) goto error;
#endif
#ifdef CLIENT_COMPRESS
    if (PyModule_AddIntMacro(constants, CLIENT_COMPRESS) < 0) goto error;
#endif
#ifdef CLIENT_ODBC
    if (PyModule_AddIntMacro(constants, CLIENT_ODBC) < 0) goto error;
#endif
#ifdef CLIENT_LOCAL_FILES
    if (PyModule_AddIntMacro(constants, CLIENT_LOCAL_FILES) < 0) goto error;
#endif
#ifdef CLIENT_IGNORE_SPACE
    if (PyModule_AddIntMacro(constants, CLIENT_IGNORE_SPACE) < 0) goto error;
#endif
#ifdef CLIENT_PROTOCOL_41
    if (PyModule_AddIntMacro(constants, CLIENT_PROTOCOL_41) < 0) goto error;
#endif
#ifdef CLIENT_INTERACTIVE
    if (PyModule_AddIntMacro(constants, CLIENT_INTERACTIVE) < 0) goto error;
#endif
#ifdef CLIENT_SSL
    if (PyModule_AddIntMacro(constants, CLIENT_SSL) < 0) goto error;
#endif
#ifdef CLIENT_IGNORE_SIGPIPE
    if (PyModule_AddIntMacro(constants, CLIENT_IGNORE_SIGPIPE) < 0) goto error;
#endif
#ifdef CLIENT_TRANSACTIONS
    if (PyModule_AddIntMacro(constants, CLIENT_TRANSACTIONS) < 0) goto error;
#endif
#ifdef CLIENT_RESERVED
    if (PyModule_AddIntMacro(constants, CLIENT_RESERVED) < 0) goto error;
#endif
#ifdef CLIENT_SECURE_CONNECTION
    if (PyModule_AddIntMacro(constants, CLIENT_SECURE_CONNECTION) < 0) goto error;
#endif
#ifdef CLIENT_MULTI_STATEMENTS
    if (PyModule_AddIntMacro(constants, CLIENT_MULTI_STATEMENTS) < 0) goto error;
#endif
#ifdef CLIENT_MULTI_RESULTS
    if (PyModule_AddIntMacro(constants, CLIENT_MULTI_RESULTS) < 0) goto error;
#endif
#ifdef CLIENT_PS_MULTI_RESULTS
    if (PyModule_AddIntMacro(constants, CLIENT_PS_MULTI_RESULTS) < 0) goto error;
#endif
#ifdef CLIENT_PLUGIN_AUTH
    if (PyModule_AddIntMacro(constants, CLIENT_PLUGIN_AUTH) < 0) goto error;
#endif
#ifdef CLIENT_CONNECT_ATTRS
    if (PyModule_AddIntMacro(constants, CLIENT_CONNECT_ATTRS) < 0) goto error;
#endif
#ifdef CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA
    if (PyModule_AddIntMacro(constants, CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA) < 0) goto error;
#endif
#ifdef CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS
    if (PyModule_AddIntMacro(constants, CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS) < 0) goto error;
#endif
#ifdef CLIENT_SSL_VERIFY_SERVER_CERT
    if (PyModule_AddIntMacro(constants, CLIENT_SSL_VERIFY_SERVER_CERT) < 0) goto error;
#endif
#ifdef CLIENT_REMEMBER_OPTIONS
    if (PyModule_AddIntMacro(constants, CLIENT_REMEMBER_OPTIONS) < 0) goto error;
#endif
#ifdef CLIENT_ALL_FLAGS
    if (PyModule_AddIntMacro(constants, CLIENT_ALL_FLAGS) < 0) goto error;
#endif
#ifdef CLIENT_BASIC_FLAGS
    if (PyModule_AddIntMacro(constants, CLIENT_BASIC_FLAGS) < 0) goto error;
#endif

//CR_*
#ifdef CR_MIN_ERROR
    if (PyModule_AddIntMacro(constants, CR_MIN_ERROR) < 0) goto error;
#endif
#ifdef CR_MAX_ERROR
    if (PyModule_AddIntMacro(constants, CR_MAX_ERROR) < 0) goto error;
#endif
#ifdef CR_ERROR_FIRST
    if (PyModule_AddIntMacro(constants, CR_ERROR_FIRST) < 0) goto error;
#endif
#ifdef CR_UNKNOWN_ERROR
    if (PyModule_AddIntMacro(constants, CR_UNKNOWN_ERROR) < 0) goto error;
#endif
#ifdef CR_SOCKET_CREATE_ERROR
    if (PyModule_AddIntMacro(constants, CR_SOCKET_CREATE_ERROR) < 0) goto error;
#endif
#ifdef CR_CONNECTION_ERROR
    if (PyModule_AddIntMacro(constants, CR_CONNECTION_ERROR) < 0) goto error;
#endif
#ifdef CR_CONN_HOST_ERROR
    if (PyModule_AddIntMacro(constants, CR_CONN_HOST_ERROR) < 0) goto error;
#endif
#ifdef CR_IPSOCK_ERROR
    if (PyModule_AddIntMacro(constants, CR_IPSOCK_ERROR) < 0) goto error;
#endif
#ifdef CR_UNKNOWN_HOST
    if (PyModule_AddIntMacro(constants, CR_UNKNOWN_HOST) < 0) goto error;
#endif
#ifdef CR_SERVER_GONE_ERROR
    if (PyModule_AddIntMacro(constants, CR_SERVER_GONE_ERROR) < 0) goto error;
#endif
#ifdef CR_VERSION_ERROR
    if (PyModule_AddIntMacro(constants, CR_VERSION_ERROR) < 0) goto error;
#endif
#ifdef CR_OUT_OF_MEMORY
    if (PyModule_AddIntMacro(constants, CR_OUT_OF_MEMORY) < 0) goto error;
#endif
#ifdef CR_WRONG_HOST_INFO
    if (PyModule_AddIntMacro(constants, CR_WRONG_HOST_INFO) < 0) goto error;
#endif
#ifdef CR_LOCALHOST_CONNECTION
    if (PyModule_AddIntMacro(constants, CR_LOCALHOST_CONNECTION) < 0) goto error;
#endif
#ifdef CR_TCP_CONNECTION
    if (PyModule_AddIntMacro(constants, CR_TCP_CONNECTION) < 0) goto error;
#endif
#ifdef CR_SERVER_HANDSHAKE_ERR
    if (PyModule_AddIntMacro(constants, CR_SERVER_HANDSHAKE_ERR) < 0) goto error;
#endif
#ifdef CR_SERVER_LOST
    if (PyModule_AddIntMacro(constants, CR_SERVER_LOST) < 0) goto error;
#endif
#ifdef CR_COMMANDS_OUT_OF_SYNC
    if (PyModule_AddIntMacro(constants, CR_COMMANDS_OUT_OF_SYNC) < 0) goto error;
#endif
#ifdef CR_NAMEDPIPE_CONNECTION
    if (PyModule_AddIntMacro(constants, CR_NAMEDPIPE_CONNECTION) < 0) goto error;
#endif
#ifdef CR_NAMEDPIPEWAIT_ERROR
    if (PyModule_AddIntMacro(constants, CR_NAMEDPIPEWAIT_ERROR) < 0) goto error;
#endif
#ifdef CR_NAMEDPIPEOPEN_ERROR
    if (PyModule_AddIntMacro(constants, CR_NAMEDPIPEOPEN_ERROR) < 0) goto error;
#endif
#ifdef CR_NAMEDPIPESETSTATE_ERROR
    if (PyModule_AddIntMacro(constants, CR_NAMEDPIPESETSTATE_ERROR) < 0) goto error;
#endif
#ifdef CR_CANT_READ_CHARSET
    if (PyModule_AddIntMacro(constants, CR_CANT_READ_CHARSET) < 0) goto error;
#endif
#ifdef CR_NET_PACKET_TOO_LARGE
    if (PyModule_AddIntMacro(constants, CR_NET_PACKET_TOO_LARGE) < 0) goto error;
#endif
#ifdef CR_EMBEDDED_CONNECTION
    if (PyModule_AddIntMacro(constants, CR_EMBEDDED_CONNECTION) < 0) goto error;
#endif
#ifdef CR_PROBE_SLAVE_STATUS
    if (PyModule_AddIntMacro(constants, CR_PROBE_SLAVE_STATUS) < 0) goto error;
#endif
#ifdef CR_PROBE_SLAVE_HOSTS
    if (PyModule_AddIntMacro(constants, CR_PROBE_SLAVE_HOSTS) < 0) goto error;
#endif
#ifdef CR_PROBE_SLAVE_CONNECT
    if (PyModule_AddIntMacro(constants, CR_PROBE_SLAVE_CONNECT) < 0) goto error;
#endif
#ifdef CR_PROBE_MASTER_CONNECT
    if (PyModule_AddIntMacro(constants, CR_PROBE_MASTER_CONNECT) < 0) goto error;
#endif
#ifdef CR_SSL_CONNECTION_ERROR
    if (PyModule_AddIntMacro(constants, CR_SSL_CONNECTION_ERROR) < 0) goto error;
#endif
#ifdef CR_MALFORMED_PACKET
    if (PyModule_AddIntMacro(constants, CR_MALFORMED_PACKET) < 0) goto error;
#endif
#ifdef CR_WRONG_LICENSE
    if (PyModule_AddIntMacro(constants, CR_WRONG_LICENSE) < 0) goto error;
#endif

#ifdef CR_NULL_POINTER
    if (PyModule_AddIntMacro(constants, CR_NULL_POINTER) < 0) goto error;
#endif
#ifdef CR_NO_PREPARE_STMT
    if (PyModule_AddIntMacro(constants, CR_NO_PREPARE_STMT) < 0) goto error;
#endif
#ifdef CR_PARAMS_NOT_BOUND
    if (PyModule_AddIntMacro(constants, CR_PARAMS_NOT_BOUND) < 0) goto error;
#endif
#ifdef CR_DATA_TRUNCATED
    if (PyModule_AddIntMacro(constants, CR_DATA_TRUNCATED) < 0) goto error;
#endif
#ifdef CR_NO_PARAMETERS_EXISTS
    if (PyModule_AddIntMacro(constants, CR_NO_PARAMETERS_EXISTS) < 0) goto error;
#endif
#ifdef CR_INVALID_PARAMETER_NO
    if (PyModule_AddIntMacro(constants, CR_INVALID_PARAMETER_NO) < 0) goto error;
#endif
#ifdef CR_INVALID_BUFFER_USE
    if (PyModule_AddIntMacro(constants, CR_INVALID_BUFFER_USE) < 0) goto error;
#endif
#ifdef CR_UNSUPPORTED_PARAM_TYPE
    if (PyModule_AddIntMacro(constants, CR_UNSUPPORTED_PARAM_TYPE) < 0) goto error;
#endif

#ifdef CR_SHARED_MEMORY_CONNECTION
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECTION) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_CONNECT_MAP_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECT_MAP_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_FILE_MAP_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_FILE_MAP_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_MAP_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_MAP_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_EVENT_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_EVENT_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR) < 0) goto error;
#endif
#ifdef CR_SHARED_MEMORY_CONNECT_SET_ERROR
    if (PyModule_AddIntMacro(constants, CR_SHARED_MEMORY_CONNECT_SET_ERROR) < 0) goto error;
#endif
#ifdef CR_CONN_UNKNOW_PROTOCOL
    if (PyModule_AddIntMacro(constants, CR_CONN_UNKNOW_PROTOCOL) < 0) goto error;
#endif
#ifdef CR_INVALID_CONN_HANDLE
    if (PyModule_AddIntMacro(constants, CR_INVALID_CONN_HANDLE) < 0) goto error;
#endif
#ifdef CR_SECURE_AUTH
    if (PyModule_AddIntMacro(constants, CR_SECURE_AUTH) < 0) goto error;
#endif
#ifdef CR_FETCH_CANCELED
    if (PyModule_AddIntMacro(constants, CR_FETCH_CANCELED) < 0) goto error;
#endif
#ifdef CR_NO_DATA
    if (PyModule_AddIntMacro(constants, CR_NO_DATA) < 0) goto error;
#endif
#ifdef CR_NO_STMT_METADATA
    if (PyModule_AddIntMacro(constants, CR_NO_STMT_METADATA) < 0) goto error;
#endif
#ifdef CR_NO_RESULT_SET
    if (PyModule_AddIntMacro(constants, CR_NO_RESULT_SET) < 0) goto error;
#endif
#ifdef CR_NOT_IMPLEMENTED
    if (PyModule_AddIntMacro(constants, CR_NOT_IMPLEMENTED) < 0) goto error;
#endif
#ifdef CR_SERVER_LOST_EXTENDED
    if (PyModule_AddIntMacro(constants, CR_SERVER_LOST_EXTENDED) < 0) goto error;
#endif
#ifdef CR_STMT_CLOSED
    if (PyModule_AddIntMacro(constants, CR_STMT_CLOSED) < 0) goto error;
#endif
#ifdef CR_NEW_STMT_METADATA
    if (PyModule_AddIntMacro(constants, CR_NEW_STMT_METADATA) < 0) goto error;
#endif
#ifdef CR_ALREADY_CONNECTED
    if (PyModule_AddIntMacro(constants, CR_ALREADY_CONNECTED) < 0) goto error;
#endif
#ifdef CR_AUTH_PLUGIN_CANNOT_LOAD
    if (PyModule_AddIntMacro(constants, CR_AUTH_PLUGIN_CANNOT_LOAD) < 0) goto error;
#endif
#ifdef CR_DUPLICATE_CONNECTION_ATTR
    if (PyModule_AddIntMacro(constants, CR_DUPLICATE_CONNECTION_ATTR) < 0) goto error;
#endif
#ifdef CR_AUTH_PLUGIN_ERR
    if (PyModule_AddIntMacro(constants, CR_AUTH_PLUGIN_ERR) < 0) goto error;
#endif
#ifdef CR_NET_READ_INTERRUPTED
    if (PyModule_AddIntMacro(constants, CR_NET_READ_INTERRUPTED) < 0) goto error;
#endif
#ifdef CR_NET_WRITE_INTERRUPTED
    if (PyModule_AddIntMacro(constants, CR_NET_WRITE_INTERRUPTED) < 0) goto error;
#endif
#ifdef CR_ERROR_LAST
    if (PyModule_AddIntMacro(constants, CR_ERROR_LAST) < 0) goto error;
#endif

    //FIELD_TYPE_*
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_DECIMAL", MYSQL_TYPE_DECIMAL) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_TINY", MYSQL_TYPE_TINY) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_SHORT", MYSQL_TYPE_SHORT) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_LONG", MYSQL_TYPE_LONG) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_FLOAT", MYSQL_TYPE_FLOAT) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_DOUBLE", MYSQL_TYPE_DOUBLE) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_NULL", MYSQL_TYPE_NULL) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_TIMESTAMP", MYSQL_TYPE_TIMESTAMP) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_LONGLONG", MYSQL_TYPE_LONGLONG) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_INT24", MYSQL_TYPE_INT24) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_DATE", MYSQL_TYPE_DATE) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_TIME", MYSQL_TYPE_TIME) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_DATETIME", MYSQL_TYPE_DATETIME) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_YEAR", MYSQL_TYPE_YEAR) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_NEWDATE", MYSQL_TYPE_NEWDATE) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_VARCHAR", MYSQL_TYPE_VARCHAR) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_BIT", MYSQL_TYPE_BIT) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_TIMESTAMP2", MYSQL_TYPE_TIMESTAMP2) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_DATETIME2", MYSQL_TYPE_DATETIME2) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_TIME2", MYSQL_TYPE_TIME2) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_DOCUMENT", MYSQL_TYPE_DOCUMENT) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_NEWDECIMAL", MYSQL_TYPE_NEWDECIMAL) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_ENUM", MYSQL_TYPE_ENUM) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_SET", MYSQL_TYPE_SET) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_TINY_BLOB", MYSQL_TYPE_TINY_BLOB) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_MEDIUM_BLOB", MYSQL_TYPE_MEDIUM_BLOB) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_LONG_BLOB", MYSQL_TYPE_LONG_BLOB) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_BLOB", MYSQL_TYPE_BLOB) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_VAR_STRING", MYSQL_TYPE_VAR_STRING) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_STRING", MYSQL_TYPE_STRING) < 0) goto error;
    if (PyModule_AddIntConstant(constants, "FIELD_TYPE_GEOMETRY", MYSQL_TYPE_GEOMETRY) < 0) goto error;

    // *_FLAG
#ifdef NOT_NULL_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_NOT_NULL", NOT_NULL_FLAG) < 0) goto error;
#endif
#ifdef PRI_KEY_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_PRI_KEY", PRI_KEY_FLAG) < 0) goto error;
#endif
#ifdef UNIQUE_KEY_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_UNIQUE_KEY", UNIQUE_KEY_FLAG) < 0) goto error;
#endif
#ifdef MULTIPLE_KEY_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_MULTIPLE_KEY", MULTIPLE_KEY_FLAG) < 0) goto error;
#endif
#ifdef BLOB_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_BLOB", BLOB_FLAG) < 0) goto error;
#endif
#ifdef UNSIGNED_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_UNSIGNED", UNSIGNED_FLAG) < 0) goto error;
#endif
#ifdef ZEROFILL_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_ZEROFILL", ZEROFILL_FLAG) < 0) goto error;
#endif
#ifdef BINARY_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_BINARY", BINARY_FLAG) < 0) goto error;
#endif
#ifdef ENUM_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_ENUM", ENUM_FLAG) < 0) goto error;
#endif
#ifdef AUTO_INCREMENT_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_AUTO_INCREMENT", AUTO_INCREMENT_FLAG) < 0) goto error;
#endif
#ifdef TIMESTAMP_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_TIMESTAMP", TIMESTAMP_FLAG) < 0) goto error;
#endif
#ifdef SET_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_SET", SET_FLAG) < 0) goto error;
#endif
#ifdef NO_DEFAULT_VALUE_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_NO_DEFAULT_VALUE", NO_DEFAULT_VALUE_FLAG) < 0) goto error;
#endif
#ifdef ON_UPDATE_NOW_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_ON_UPDATE_NOW", ON_UPDATE_NOW_FLAG) < 0) goto error;
#endif
#ifdef NUM_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_NUM", NUM_FLAG) < 0) goto error;
#endif
#ifdef PART_KEY_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_PART_KEY", PART_KEY_FLAG) < 0) goto error;
#endif
#ifdef GROUP_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_GROUP", GROUP_FLAG) < 0) goto error;
#endif
#ifdef UNIQUE_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_UNIQUE", UNIQUE_FLAG) < 0) goto error;
#endif
#ifdef BINCMP_FLAG
    if (PyModule_AddIntConstant(constants, "FLAG_BINCMP", BINCMP_FLAG) < 0) goto error;
#endif

    // REFRESH_*
#ifdef REFRESH_GRANT
    if (PyModule_AddIntMacro(constants, REFRESH_GRANT) < 0) goto error;
#endif
#ifdef REFRESH_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_LOG) < 0) goto error;
#endif
#ifdef REFRESH_TABLES
    if (PyModule_AddIntMacro(constants, REFRESH_TABLES) < 0) goto error;
#endif
#ifdef REFRESH_HOSTS
    if (PyModule_AddIntMacro(constants, REFRESH_HOSTS) < 0) goto error;
#endif
#ifdef REFRESH_STATUS
    if (PyModule_AddIntMacro(constants, REFRESH_STATUS) < 0) goto error;
#endif
#ifdef REFRESH_THREADS
    if (PyModule_AddIntMacro(constants, REFRESH_THREADS) < 0) goto error;
#endif
#ifdef REFRESH_SLAVE
    if (PyModule_AddIntMacro(constants, REFRESH_SLAVE) < 0) goto error;
#endif
#ifdef REFRESH_MASTER
    if (PyModule_AddIntMacro(constants, REFRESH_MASTER) < 0) goto error;
#endif
#ifdef REFRESH_ERROR_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_ERROR_LOG) < 0) goto error;
#endif
#ifdef REFRESH_ENGINE_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_ENGINE_LOG) < 0) goto error;
#endif
#ifdef REFRESH_BINARY_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_BINARY_LOG) < 0) goto error;
#endif
#ifdef REFRESH_RELAY_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_RELAY_LOG) < 0) goto error;
#endif
#ifdef REFRESH_GENERAL_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_GENERAL_LOG) < 0) goto error;
#endif
#ifdef REFRESH_SLOW_LOG
    if (PyModule_AddIntMacro(constants, REFRESH_SLOW_LOG) < 0) goto error;
#endif
#ifdef REFRESH_READ_LOCK
    if (PyModule_AddIntMacro(constants, REFRESH_READ_LOCK) < 0) goto error;
#endif
#ifdef REFRESH_FAST
    if (PyModule_AddIntMacro(constants, REFRESH_FAST) < 0) goto error;
#endif
#ifdef REFRESH_QUERY_CACHE
    if (PyModule_AddIntMacro(constants, REFRESH_QUERY_CACHE) < 0) goto error;
#endif
#ifdef REFRESH_QUERY_CACHE_FREE
    if (PyModule_AddIntMacro(constants, REFRESH_QUERY_CACHE_FREE) < 0) goto error;
#endif
#ifdef REFRESH_DES_KEY_FILE
    if (PyModule_AddIntMacro(constants, REFRESH_DES_KEY_FILE) < 0) goto error;
#endif
#ifdef REFRESH_USER_RESOURCES
    if (PyModule_AddIntMacro(constants, REFRESH_USER_RESOURCES) < 0) goto error;
#endif
#ifdef REFRESH_FOR_EXPORT
    if (PyModule_AddIntMacro(constants, REFRESH_FOR_EXPORT) < 0) goto error;
#endif
#ifdef REFRESH_STATISTICS
    if (PyModule_AddIntMacro(constants, REFRESH_STATISTICS) < 0) goto error;
#endif

    if (PyModule_AddObject(module, "constants", constants) < 0) goto error;

    return 0;

  error:
    Py_XDECREF(constants);
    return -1;
}
