#include "mysqlmod.h"

const char _mysql_constants__doc__[] =
"Various constants used by the MySQL client/server protocol.\n";

#ifdef PY3K
PyModuleDef _mysql_constants_module =
{
    PyModuleDef_HEAD_INIT,
    "_mysql.constants",        /* name of module */
    _mysql_constants__doc__,   /* module documentation, may be NULL */
    -1,                        /* size of per-interpreter state of the module,
                                or -1 if the module keeps state in global variables. */
    NULL, NULL, NULL, NULL, NULL
};
#endif  // PY3K

int _mysql_constants_add(
    PyObject* module)
{
	PyObject *constants = NULL;
#ifdef PY3K
    constants = PyModule_Create(&_mysql_constants_module);
#else
	constants = Py_InitModule3("_mysql.constants", NULL, _mysql_constants__doc__);
#endif
    if (!constants)
        return -1;

    if (PyModule_AddStringConstant(constants, "NULL", "NULL") < 0) goto error;

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

//ER_*
#ifdef ER_ERROR_FIRST
    if (PyModule_AddIntMacro(constants, ER_ERROR_FIRST) < 0) goto error;
#endif
#ifdef ER_HASHCHK
    if (PyModule_AddIntMacro(constants, ER_HASHCHK) < 0) goto error;
#endif
#ifdef ER_NISAMCHK
    if (PyModule_AddIntMacro(constants, ER_NISAMCHK) < 0) goto error;
#endif
#ifdef ER_NO
    if (PyModule_AddIntMacro(constants, ER_NO) < 0) goto error;
#endif
#ifdef ER_YES
    if (PyModule_AddIntMacro(constants, ER_YES) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_FILE
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_FILE) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_TABLE
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_TABLE) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_DB
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_DB) < 0) goto error;
#endif
#ifdef ER_DB_CREATE_EXISTS
    if (PyModule_AddIntMacro(constants, ER_DB_CREATE_EXISTS) < 0) goto error;
#endif
#ifdef ER_DB_DROP_EXISTS
    if (PyModule_AddIntMacro(constants, ER_DB_DROP_EXISTS) < 0) goto error;
#endif
#ifdef ER_DB_DROP_DELETE
    if (PyModule_AddIntMacro(constants, ER_DB_DROP_DELETE) < 0) goto error;
#endif
#ifdef ER_DB_DROP_RMDIR
    if (PyModule_AddIntMacro(constants, ER_DB_DROP_RMDIR) < 0) goto error;
#endif
#ifdef ER_CANT_DELETE_FILE
    if (PyModule_AddIntMacro(constants, ER_CANT_DELETE_FILE) < 0) goto error;
#endif
#ifdef ER_CANT_FIND_SYSTEM_REC
    if (PyModule_AddIntMacro(constants, ER_CANT_FIND_SYSTEM_REC) < 0) goto error;
#endif
#ifdef ER_CANT_GET_STAT
    if (PyModule_AddIntMacro(constants, ER_CANT_GET_STAT) < 0) goto error;
#endif
#ifdef ER_CANT_GET_WD
    if (PyModule_AddIntMacro(constants, ER_CANT_GET_WD) < 0) goto error;
#endif
#ifdef ER_CANT_LOCK
    if (PyModule_AddIntMacro(constants, ER_CANT_LOCK) < 0) goto error;
#endif
#ifdef ER_CANT_OPEN_FILE
    if (PyModule_AddIntMacro(constants, ER_CANT_OPEN_FILE) < 0) goto error;
#endif
#ifdef ER_FILE_NOT_FOUND
    if (PyModule_AddIntMacro(constants, ER_FILE_NOT_FOUND) < 0) goto error;
#endif
#ifdef ER_CANT_READ_DIR
    if (PyModule_AddIntMacro(constants, ER_CANT_READ_DIR) < 0) goto error;
#endif
#ifdef ER_CANT_SET_WD
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_WD) < 0) goto error;
#endif
#ifdef ER_CHECKREAD
    if (PyModule_AddIntMacro(constants, ER_CHECKREAD) < 0) goto error;
#endif
#ifdef ER_DISK_FULL
    if (PyModule_AddIntMacro(constants, ER_DISK_FULL) < 0) goto error;
#endif
#ifdef ER_DUP_KEY
    if (PyModule_AddIntMacro(constants, ER_DUP_KEY) < 0) goto error;
#endif
#ifdef ER_ERROR_ON_CLOSE
    if (PyModule_AddIntMacro(constants, ER_ERROR_ON_CLOSE) < 0) goto error;
#endif
#ifdef ER_ERROR_ON_READ
    if (PyModule_AddIntMacro(constants, ER_ERROR_ON_READ) < 0) goto error;
#endif
#ifdef ER_ERROR_ON_RENAME
    if (PyModule_AddIntMacro(constants, ER_ERROR_ON_RENAME) < 0) goto error;
#endif
#ifdef ER_ERROR_ON_WRITE
    if (PyModule_AddIntMacro(constants, ER_ERROR_ON_WRITE) < 0) goto error;
#endif
#ifdef ER_FILE_USED
    if (PyModule_AddIntMacro(constants, ER_FILE_USED) < 0) goto error;
#endif
#ifdef ER_FILSORT_ABORT
    if (PyModule_AddIntMacro(constants, ER_FILSORT_ABORT) < 0) goto error;
#endif
#ifdef ER_FORM_NOT_FOUND
    if (PyModule_AddIntMacro(constants, ER_FORM_NOT_FOUND) < 0) goto error;
#endif
#ifdef ER_GET_ERRNO
    if (PyModule_AddIntMacro(constants, ER_GET_ERRNO) < 0) goto error;
#endif
#ifdef ER_ILLEGAL_HA
    if (PyModule_AddIntMacro(constants, ER_ILLEGAL_HA) < 0) goto error;
#endif
#ifdef ER_KEY_NOT_FOUND
    if (PyModule_AddIntMacro(constants, ER_KEY_NOT_FOUND) < 0) goto error;
#endif
#ifdef ER_NOT_FORM_FILE
    if (PyModule_AddIntMacro(constants, ER_NOT_FORM_FILE) < 0) goto error;
#endif
#ifdef ER_NOT_KEYFILE
    if (PyModule_AddIntMacro(constants, ER_NOT_KEYFILE) < 0) goto error;
#endif
#ifdef ER_OLD_KEYFILE
    if (PyModule_AddIntMacro(constants, ER_OLD_KEYFILE) < 0) goto error;
#endif
#ifdef ER_OPEN_AS_READONLY
    if (PyModule_AddIntMacro(constants, ER_OPEN_AS_READONLY) < 0) goto error;
#endif
#ifdef ER_OUTOFMEMORY
    if (PyModule_AddIntMacro(constants, ER_OUTOFMEMORY) < 0) goto error;
#endif
#ifdef ER_OUT_OF_SORTMEMORY
    if (PyModule_AddIntMacro(constants, ER_OUT_OF_SORTMEMORY) < 0) goto error;
#endif
#ifdef ER_UNEXPECTED_EOF
    if (PyModule_AddIntMacro(constants, ER_UNEXPECTED_EOF) < 0) goto error;
#endif
#ifdef ER_CON_COUNT_ERROR
    if (PyModule_AddIntMacro(constants, ER_CON_COUNT_ERROR) < 0) goto error;
#endif
#ifdef ER_OUT_OF_RESOURCES
    if (PyModule_AddIntMacro(constants, ER_OUT_OF_RESOURCES) < 0) goto error;
#endif
#ifdef ER_BAD_HOST_ERROR
    if (PyModule_AddIntMacro(constants, ER_BAD_HOST_ERROR) < 0) goto error;
#endif
#ifdef ER_HANDSHAKE_ERROR
    if (PyModule_AddIntMacro(constants, ER_HANDSHAKE_ERROR) < 0) goto error;
#endif
#ifdef ER_DBACCESS_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_DBACCESS_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_ACCESS_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_ACCESS_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_NO_DB_ERROR
    if (PyModule_AddIntMacro(constants, ER_NO_DB_ERROR) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_COM_ERROR
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_COM_ERROR) < 0) goto error;
#endif
#ifdef ER_BAD_NULL_ERROR
    if (PyModule_AddIntMacro(constants, ER_BAD_NULL_ERROR) < 0) goto error;
#endif
#ifdef ER_BAD_DB_ERROR
    if (PyModule_AddIntMacro(constants, ER_BAD_DB_ERROR) < 0) goto error;
#endif
#ifdef ER_TABLE_EXISTS_ERROR
    if (PyModule_AddIntMacro(constants, ER_TABLE_EXISTS_ERROR) < 0) goto error;
#endif
#ifdef ER_BAD_TABLE_ERROR
    if (PyModule_AddIntMacro(constants, ER_BAD_TABLE_ERROR) < 0) goto error;
#endif
#ifdef ER_NON_UNIQ_ERROR
    if (PyModule_AddIntMacro(constants, ER_NON_UNIQ_ERROR) < 0) goto error;
#endif
#ifdef ER_SERVER_SHUTDOWN
    if (PyModule_AddIntMacro(constants, ER_SERVER_SHUTDOWN) < 0) goto error;
#endif
#ifdef ER_BAD_FIELD_ERROR
    if (PyModule_AddIntMacro(constants, ER_BAD_FIELD_ERROR) < 0) goto error;
#endif
#ifdef ER_WRONG_FIELD_WITH_GROUP
    if (PyModule_AddIntMacro(constants, ER_WRONG_FIELD_WITH_GROUP) < 0) goto error;
#endif
#ifdef ER_WRONG_GROUP_FIELD
    if (PyModule_AddIntMacro(constants, ER_WRONG_GROUP_FIELD) < 0) goto error;
#endif
#ifdef ER_WRONG_SUM_SELECT
    if (PyModule_AddIntMacro(constants, ER_WRONG_SUM_SELECT) < 0) goto error;
#endif
#ifdef ER_WRONG_VALUE_COUNT
    if (PyModule_AddIntMacro(constants, ER_WRONG_VALUE_COUNT) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_IDENT
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_IDENT) < 0) goto error;
#endif
#ifdef ER_DUP_FIELDNAME
    if (PyModule_AddIntMacro(constants, ER_DUP_FIELDNAME) < 0) goto error;
#endif
#ifdef ER_DUP_KEYNAME
    if (PyModule_AddIntMacro(constants, ER_DUP_KEYNAME) < 0) goto error;
#endif
#ifdef ER_DUP_ENTRY
    if (PyModule_AddIntMacro(constants, ER_DUP_ENTRY) < 0) goto error;
#endif
#ifdef ER_WRONG_FIELD_SPEC
    if (PyModule_AddIntMacro(constants, ER_WRONG_FIELD_SPEC) < 0) goto error;
#endif
#ifdef ER_PARSE_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARSE_ERROR) < 0) goto error;
#endif
#ifdef ER_EMPTY_QUERY
    if (PyModule_AddIntMacro(constants, ER_EMPTY_QUERY) < 0) goto error;
#endif
#ifdef ER_NONUNIQ_TABLE
    if (PyModule_AddIntMacro(constants, ER_NONUNIQ_TABLE) < 0) goto error;
#endif
#ifdef ER_INVALID_DEFAULT
    if (PyModule_AddIntMacro(constants, ER_INVALID_DEFAULT) < 0) goto error;
#endif
#ifdef ER_MULTIPLE_PRI_KEY
    if (PyModule_AddIntMacro(constants, ER_MULTIPLE_PRI_KEY) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_KEYS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_KEYS) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_KEY_PARTS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_KEY_PARTS) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_KEY
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_KEY) < 0) goto error;
#endif
#ifdef ER_KEY_COLUMN_DOES_NOT_EXITS
    if (PyModule_AddIntMacro(constants, ER_KEY_COLUMN_DOES_NOT_EXITS) < 0) goto error;
#endif
#ifdef ER_BLOB_USED_AS_KEY
    if (PyModule_AddIntMacro(constants, ER_BLOB_USED_AS_KEY) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_FIELDLENGTH
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_FIELDLENGTH) < 0) goto error;
#endif
#ifdef ER_WRONG_AUTO_KEY
    if (PyModule_AddIntMacro(constants, ER_WRONG_AUTO_KEY) < 0) goto error;
#endif
#ifdef ER_READY
    if (PyModule_AddIntMacro(constants, ER_READY) < 0) goto error;
#endif
#ifdef ER_NORMAL_SHUTDOWN
    if (PyModule_AddIntMacro(constants, ER_NORMAL_SHUTDOWN) < 0) goto error;
#endif
#ifdef ER_GOT_SIGNAL
    if (PyModule_AddIntMacro(constants, ER_GOT_SIGNAL) < 0) goto error;
#endif
#ifdef ER_SHUTDOWN_COMPLETE
    if (PyModule_AddIntMacro(constants, ER_SHUTDOWN_COMPLETE) < 0) goto error;
#endif
#ifdef ER_FORCING_CLOSE
    if (PyModule_AddIntMacro(constants, ER_FORCING_CLOSE) < 0) goto error;
#endif
#ifdef ER_IPSOCK_ERROR
    if (PyModule_AddIntMacro(constants, ER_IPSOCK_ERROR) < 0) goto error;
#endif
#ifdef ER_NO_SUCH_INDEX
    if (PyModule_AddIntMacro(constants, ER_NO_SUCH_INDEX) < 0) goto error;
#endif
#ifdef ER_WRONG_FIELD_TERMINATORS
    if (PyModule_AddIntMacro(constants, ER_WRONG_FIELD_TERMINATORS) < 0) goto error;
#endif
#ifdef ER_BLOBS_AND_NO_TERMINATED
    if (PyModule_AddIntMacro(constants, ER_BLOBS_AND_NO_TERMINATED) < 0) goto error;
#endif
#ifdef ER_TEXTFILE_NOT_READABLE
    if (PyModule_AddIntMacro(constants, ER_TEXTFILE_NOT_READABLE) < 0) goto error;
#endif
#ifdef ER_FILE_EXISTS_ERROR
    if (PyModule_AddIntMacro(constants, ER_FILE_EXISTS_ERROR) < 0) goto error;
#endif
#ifdef ER_LOAD_INFO
    if (PyModule_AddIntMacro(constants, ER_LOAD_INFO) < 0) goto error;
#endif
#ifdef ER_ALTER_INFO
    if (PyModule_AddIntMacro(constants, ER_ALTER_INFO) < 0) goto error;
#endif
#ifdef ER_WRONG_SUB_KEY
    if (PyModule_AddIntMacro(constants, ER_WRONG_SUB_KEY) < 0) goto error;
#endif
#ifdef ER_CANT_REMOVE_ALL_FIELDS
    if (PyModule_AddIntMacro(constants, ER_CANT_REMOVE_ALL_FIELDS) < 0) goto error;
#endif
#ifdef ER_CANT_DROP_FIELD_OR_KEY
    if (PyModule_AddIntMacro(constants, ER_CANT_DROP_FIELD_OR_KEY) < 0) goto error;
#endif
#ifdef ER_INSERT_INFO
    if (PyModule_AddIntMacro(constants, ER_INSERT_INFO) < 0) goto error;
#endif
#ifdef ER_UPDATE_TABLE_USED
    if (PyModule_AddIntMacro(constants, ER_UPDATE_TABLE_USED) < 0) goto error;
#endif
#ifdef ER_NO_SUCH_THREAD
    if (PyModule_AddIntMacro(constants, ER_NO_SUCH_THREAD) < 0) goto error;
#endif
#ifdef ER_KILL_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_KILL_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_NO_TABLES_USED
    if (PyModule_AddIntMacro(constants, ER_NO_TABLES_USED) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_SET
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_SET) < 0) goto error;
#endif
#ifdef ER_NO_UNIQUE_LOGFILE
    if (PyModule_AddIntMacro(constants, ER_NO_UNIQUE_LOGFILE) < 0) goto error;
#endif
#ifdef ER_TABLE_NOT_LOCKED_FOR_WRITE
    if (PyModule_AddIntMacro(constants, ER_TABLE_NOT_LOCKED_FOR_WRITE) < 0) goto error;
#endif
#ifdef ER_TABLE_NOT_LOCKED
    if (PyModule_AddIntMacro(constants, ER_TABLE_NOT_LOCKED) < 0) goto error;
#endif
#ifdef ER_BLOB_CANT_HAVE_DEFAULT
    if (PyModule_AddIntMacro(constants, ER_BLOB_CANT_HAVE_DEFAULT) < 0) goto error;
#endif
#ifdef ER_WRONG_DB_NAME
    if (PyModule_AddIntMacro(constants, ER_WRONG_DB_NAME) < 0) goto error;
#endif
#ifdef ER_WRONG_TABLE_NAME
    if (PyModule_AddIntMacro(constants, ER_WRONG_TABLE_NAME) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_SELECT
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_SELECT) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_ERROR
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_ERROR) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_PROCEDURE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_PROCEDURE) < 0) goto error;
#endif
#ifdef ER_WRONG_PARAMCOUNT_TO_PROCEDURE
    if (PyModule_AddIntMacro(constants, ER_WRONG_PARAMCOUNT_TO_PROCEDURE) < 0) goto error;
#endif
#ifdef ER_WRONG_PARAMETERS_TO_PROCEDURE
    if (PyModule_AddIntMacro(constants, ER_WRONG_PARAMETERS_TO_PROCEDURE) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_TABLE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_TABLE) < 0) goto error;
#endif
#ifdef ER_FIELD_SPECIFIED_TWICE
    if (PyModule_AddIntMacro(constants, ER_FIELD_SPECIFIED_TWICE) < 0) goto error;
#endif
#ifdef ER_INVALID_GROUP_FUNC_USE
    if (PyModule_AddIntMacro(constants, ER_INVALID_GROUP_FUNC_USE) < 0) goto error;
#endif
#ifdef ER_UNSUPPORTED_EXTENSION
    if (PyModule_AddIntMacro(constants, ER_UNSUPPORTED_EXTENSION) < 0) goto error;
#endif
#ifdef ER_TABLE_MUST_HAVE_COLUMNS
    if (PyModule_AddIntMacro(constants, ER_TABLE_MUST_HAVE_COLUMNS) < 0) goto error;
#endif
#ifdef ER_RECORD_FILE_FULL
    if (PyModule_AddIntMacro(constants, ER_RECORD_FILE_FULL) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_CHARACTER_SET
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_CHARACTER_SET) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_TABLES
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_TABLES) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_FIELDS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_FIELDS) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_ROWSIZE
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_ROWSIZE) < 0) goto error;
#endif
#ifdef ER_STACK_OVERRUN
    if (PyModule_AddIntMacro(constants, ER_STACK_OVERRUN) < 0) goto error;
#endif
#ifdef ER_WRONG_OUTER_JOIN
    if (PyModule_AddIntMacro(constants, ER_WRONG_OUTER_JOIN) < 0) goto error;
#endif
#ifdef ER_NULL_COLUMN_IN_INDEX
    if (PyModule_AddIntMacro(constants, ER_NULL_COLUMN_IN_INDEX) < 0) goto error;
#endif
#ifdef ER_CANT_FIND_UDF
    if (PyModule_AddIntMacro(constants, ER_CANT_FIND_UDF) < 0) goto error;
#endif
#ifdef ER_CANT_INITIALIZE_UDF
    if (PyModule_AddIntMacro(constants, ER_CANT_INITIALIZE_UDF) < 0) goto error;
#endif
#ifdef ER_UDF_NO_PATHS
    if (PyModule_AddIntMacro(constants, ER_UDF_NO_PATHS) < 0) goto error;
#endif
#ifdef ER_UDF_EXISTS
    if (PyModule_AddIntMacro(constants, ER_UDF_EXISTS) < 0) goto error;
#endif
#ifdef ER_CANT_OPEN_LIBRARY
    if (PyModule_AddIntMacro(constants, ER_CANT_OPEN_LIBRARY) < 0) goto error;
#endif
#ifdef ER_CANT_FIND_DL_ENTRY
    if (PyModule_AddIntMacro(constants, ER_CANT_FIND_DL_ENTRY) < 0) goto error;
#endif
#ifdef ER_FUNCTION_NOT_DEFINED
    if (PyModule_AddIntMacro(constants, ER_FUNCTION_NOT_DEFINED) < 0) goto error;
#endif
#ifdef ER_HOST_IS_BLOCKED
    if (PyModule_AddIntMacro(constants, ER_HOST_IS_BLOCKED) < 0) goto error;
#endif
#ifdef ER_HOST_NOT_PRIVILEGED
    if (PyModule_AddIntMacro(constants, ER_HOST_NOT_PRIVILEGED) < 0) goto error;
#endif
#ifdef ER_PASSWORD_ANONYMOUS_USER
    if (PyModule_AddIntMacro(constants, ER_PASSWORD_ANONYMOUS_USER) < 0) goto error;
#endif
#ifdef ER_PASSWORD_NOT_ALLOWED
    if (PyModule_AddIntMacro(constants, ER_PASSWORD_NOT_ALLOWED) < 0) goto error;
#endif
#ifdef ER_PASSWORD_NO_MATCH
    if (PyModule_AddIntMacro(constants, ER_PASSWORD_NO_MATCH) < 0) goto error;
#endif
#ifdef ER_UPDATE_INFO
    if (PyModule_AddIntMacro(constants, ER_UPDATE_INFO) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_THREAD
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_THREAD) < 0) goto error;
#endif
#ifdef ER_WRONG_VALUE_COUNT_ON_ROW
    if (PyModule_AddIntMacro(constants, ER_WRONG_VALUE_COUNT_ON_ROW) < 0) goto error;
#endif
#ifdef ER_CANT_REOPEN_TABLE
    if (PyModule_AddIntMacro(constants, ER_CANT_REOPEN_TABLE) < 0) goto error;
#endif
#ifdef ER_INVALID_USE_OF_NULL
    if (PyModule_AddIntMacro(constants, ER_INVALID_USE_OF_NULL) < 0) goto error;
#endif
#ifdef ER_REGEXP_ERROR
    if (PyModule_AddIntMacro(constants, ER_REGEXP_ERROR) < 0) goto error;
#endif
#ifdef ER_MIX_OF_GROUP_FUNC_AND_FIELDS
    if (PyModule_AddIntMacro(constants, ER_MIX_OF_GROUP_FUNC_AND_FIELDS) < 0) goto error;
#endif
#ifdef ER_NONEXISTING_GRANT
    if (PyModule_AddIntMacro(constants, ER_NONEXISTING_GRANT) < 0) goto error;
#endif
#ifdef ER_TABLEACCESS_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_TABLEACCESS_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_COLUMNACCESS_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_COLUMNACCESS_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_ILLEGAL_GRANT_FOR_TABLE
    if (PyModule_AddIntMacro(constants, ER_ILLEGAL_GRANT_FOR_TABLE) < 0) goto error;
#endif
#ifdef ER_GRANT_WRONG_HOST_OR_USER
    if (PyModule_AddIntMacro(constants, ER_GRANT_WRONG_HOST_OR_USER) < 0) goto error;
#endif
#ifdef ER_NO_SUCH_TABLE
    if (PyModule_AddIntMacro(constants, ER_NO_SUCH_TABLE) < 0) goto error;
#endif
#ifdef ER_NONEXISTING_TABLE_GRANT
    if (PyModule_AddIntMacro(constants, ER_NONEXISTING_TABLE_GRANT) < 0) goto error;
#endif
#ifdef ER_NOT_ALLOWED_COMMAND
    if (PyModule_AddIntMacro(constants, ER_NOT_ALLOWED_COMMAND) < 0) goto error;
#endif
#ifdef ER_SYNTAX_ERROR
    if (PyModule_AddIntMacro(constants, ER_SYNTAX_ERROR) < 0) goto error;
#endif
#ifdef ER_DELAYED_CANT_CHANGE_LOCK
    if (PyModule_AddIntMacro(constants, ER_DELAYED_CANT_CHANGE_LOCK) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_DELAYED_THREADS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_DELAYED_THREADS) < 0) goto error;
#endif
#ifdef ER_ABORTING_CONNECTION
    if (PyModule_AddIntMacro(constants, ER_ABORTING_CONNECTION) < 0) goto error;
#endif
#ifdef ER_NET_PACKET_TOO_LARGE
    if (PyModule_AddIntMacro(constants, ER_NET_PACKET_TOO_LARGE) < 0) goto error;
#endif
#ifdef ER_NET_READ_ERROR_FROM_PIPE
    if (PyModule_AddIntMacro(constants, ER_NET_READ_ERROR_FROM_PIPE) < 0) goto error;
#endif
#ifdef ER_NET_FCNTL_ERROR
    if (PyModule_AddIntMacro(constants, ER_NET_FCNTL_ERROR) < 0) goto error;
#endif
#ifdef ER_NET_PACKETS_OUT_OF_ORDER
    if (PyModule_AddIntMacro(constants, ER_NET_PACKETS_OUT_OF_ORDER) < 0) goto error;
#endif
#ifdef ER_NET_UNCOMPRESS_ERROR
    if (PyModule_AddIntMacro(constants, ER_NET_UNCOMPRESS_ERROR) < 0) goto error;
#endif
#ifdef ER_NET_READ_ERROR
    if (PyModule_AddIntMacro(constants, ER_NET_READ_ERROR) < 0) goto error;
#endif
#ifdef ER_NET_READ_INTERRUPTED
    if (PyModule_AddIntMacro(constants, ER_NET_READ_INTERRUPTED) < 0) goto error;
#endif
#ifdef ER_NET_ERROR_ON_WRITE
    if (PyModule_AddIntMacro(constants, ER_NET_ERROR_ON_WRITE) < 0) goto error;
#endif
#ifdef ER_NET_WRITE_INTERRUPTED
    if (PyModule_AddIntMacro(constants, ER_NET_WRITE_INTERRUPTED) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_STRING
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_STRING) < 0) goto error;
#endif
#ifdef ER_TABLE_CANT_HANDLE_BLOB
    if (PyModule_AddIntMacro(constants, ER_TABLE_CANT_HANDLE_BLOB) < 0) goto error;
#endif
#ifdef ER_TABLE_CANT_HANDLE_AUTO_INCREMENT
    if (PyModule_AddIntMacro(constants, ER_TABLE_CANT_HANDLE_AUTO_INCREMENT) < 0) goto error;
#endif
#ifdef ER_DELAYED_INSERT_TABLE_LOCKED
    if (PyModule_AddIntMacro(constants, ER_DELAYED_INSERT_TABLE_LOCKED) < 0) goto error;
#endif
#ifdef ER_WRONG_COLUMN_NAME
    if (PyModule_AddIntMacro(constants, ER_WRONG_COLUMN_NAME) < 0) goto error;
#endif
#ifdef ER_WRONG_KEY_COLUMN
    if (PyModule_AddIntMacro(constants, ER_WRONG_KEY_COLUMN) < 0) goto error;
#endif
#ifdef ER_WRONG_MRG_TABLE
    if (PyModule_AddIntMacro(constants, ER_WRONG_MRG_TABLE) < 0) goto error;
#endif
#ifdef ER_DUP_UNIQUE
    if (PyModule_AddIntMacro(constants, ER_DUP_UNIQUE) < 0) goto error;
#endif
#ifdef ER_BLOB_KEY_WITHOUT_LENGTH
    if (PyModule_AddIntMacro(constants, ER_BLOB_KEY_WITHOUT_LENGTH) < 0) goto error;
#endif
#ifdef ER_PRIMARY_CANT_HAVE_NULL
    if (PyModule_AddIntMacro(constants, ER_PRIMARY_CANT_HAVE_NULL) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_ROWS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_ROWS) < 0) goto error;
#endif
#ifdef ER_REQUIRES_PRIMARY_KEY
    if (PyModule_AddIntMacro(constants, ER_REQUIRES_PRIMARY_KEY) < 0) goto error;
#endif
#ifdef ER_NO_RAID_COMPILED
    if (PyModule_AddIntMacro(constants, ER_NO_RAID_COMPILED) < 0) goto error;
#endif
#ifdef ER_UPDATE_WITHOUT_KEY_IN_SAFE_MODE
    if (PyModule_AddIntMacro(constants, ER_UPDATE_WITHOUT_KEY_IN_SAFE_MODE) < 0) goto error;
#endif
#ifdef ER_KEY_DOES_NOT_EXITS
    if (PyModule_AddIntMacro(constants, ER_KEY_DOES_NOT_EXITS) < 0) goto error;
#endif
#ifdef ER_CHECK_NO_SUCH_TABLE
    if (PyModule_AddIntMacro(constants, ER_CHECK_NO_SUCH_TABLE) < 0) goto error;
#endif
#ifdef ER_CHECK_NOT_IMPLEMENTED
    if (PyModule_AddIntMacro(constants, ER_CHECK_NOT_IMPLEMENTED) < 0) goto error;
#endif
#ifdef ER_CANT_DO_THIS_DURING_AN_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_CANT_DO_THIS_DURING_AN_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_ERROR_DURING_COMMIT
    if (PyModule_AddIntMacro(constants, ER_ERROR_DURING_COMMIT) < 0) goto error;
#endif
#ifdef ER_ERROR_DURING_ROLLBACK
    if (PyModule_AddIntMacro(constants, ER_ERROR_DURING_ROLLBACK) < 0) goto error;
#endif
#ifdef ER_ERROR_DURING_FLUSH_LOGS
    if (PyModule_AddIntMacro(constants, ER_ERROR_DURING_FLUSH_LOGS) < 0) goto error;
#endif
#ifdef ER_ERROR_DURING_CHECKPOINT
    if (PyModule_AddIntMacro(constants, ER_ERROR_DURING_CHECKPOINT) < 0) goto error;
#endif
#ifdef ER_NEW_ABORTING_CONNECTION
    if (PyModule_AddIntMacro(constants, ER_NEW_ABORTING_CONNECTION) < 0) goto error;
#endif
#ifdef ER_DUMP_NOT_IMPLEMENTED
    if (PyModule_AddIntMacro(constants, ER_DUMP_NOT_IMPLEMENTED) < 0) goto error;
#endif
#ifdef ER_FLUSH_MASTER_BINLOG_CLOSED
    if (PyModule_AddIntMacro(constants, ER_FLUSH_MASTER_BINLOG_CLOSED) < 0) goto error;
#endif
#ifdef ER_INDEX_REBUILD
    if (PyModule_AddIntMacro(constants, ER_INDEX_REBUILD) < 0) goto error;
#endif
#ifdef ER_MASTER
    if (PyModule_AddIntMacro(constants, ER_MASTER) < 0) goto error;
#endif
#ifdef ER_MASTER_NET_READ
    if (PyModule_AddIntMacro(constants, ER_MASTER_NET_READ) < 0) goto error;
#endif
#ifdef ER_MASTER_NET_WRITE
    if (PyModule_AddIntMacro(constants, ER_MASTER_NET_WRITE) < 0) goto error;
#endif
#ifdef ER_FT_MATCHING_KEY_NOT_FOUND
    if (PyModule_AddIntMacro(constants, ER_FT_MATCHING_KEY_NOT_FOUND) < 0) goto error;
#endif
#ifdef ER_LOCK_OR_ACTIVE_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_LOCK_OR_ACTIVE_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_SYSTEM_VARIABLE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_SYSTEM_VARIABLE) < 0) goto error;
#endif
#ifdef ER_CRASHED_ON_USAGE
    if (PyModule_AddIntMacro(constants, ER_CRASHED_ON_USAGE) < 0) goto error;
#endif
#ifdef ER_CRASHED_ON_REPAIR
    if (PyModule_AddIntMacro(constants, ER_CRASHED_ON_REPAIR) < 0) goto error;
#endif
#ifdef ER_WARNING_NOT_COMPLETE_ROLLBACK
    if (PyModule_AddIntMacro(constants, ER_WARNING_NOT_COMPLETE_ROLLBACK) < 0) goto error;
#endif
#ifdef ER_TRANS_CACHE_FULL
    if (PyModule_AddIntMacro(constants, ER_TRANS_CACHE_FULL) < 0) goto error;
#endif
#ifdef ER_SLAVE_MUST_STOP
    if (PyModule_AddIntMacro(constants, ER_SLAVE_MUST_STOP) < 0) goto error;
#endif
#ifdef ER_SLAVE_NOT_RUNNING
    if (PyModule_AddIntMacro(constants, ER_SLAVE_NOT_RUNNING) < 0) goto error;
#endif
#ifdef ER_BAD_SLAVE
    if (PyModule_AddIntMacro(constants, ER_BAD_SLAVE) < 0) goto error;
#endif
#ifdef ER_MASTER_INFO
    if (PyModule_AddIntMacro(constants, ER_MASTER_INFO) < 0) goto error;
#endif
#ifdef ER_SLAVE_THREAD
    if (PyModule_AddIntMacro(constants, ER_SLAVE_THREAD) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_USER_CONNECTIONS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_USER_CONNECTIONS) < 0) goto error;
#endif
#ifdef ER_SET_CONSTANTS_ONLY
    if (PyModule_AddIntMacro(constants, ER_SET_CONSTANTS_ONLY) < 0) goto error;
#endif
#ifdef ER_LOCK_WAIT_TIMEOUT
    if (PyModule_AddIntMacro(constants, ER_LOCK_WAIT_TIMEOUT) < 0) goto error;
#endif
#ifdef ER_LOCK_TABLE_FULL
    if (PyModule_AddIntMacro(constants, ER_LOCK_TABLE_FULL) < 0) goto error;
#endif
#ifdef ER_READ_ONLY_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_READ_ONLY_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_DROP_DB_WITH_READ_LOCK
    if (PyModule_AddIntMacro(constants, ER_DROP_DB_WITH_READ_LOCK) < 0) goto error;
#endif
#ifdef ER_CREATE_DB_WITH_READ_LOCK
    if (PyModule_AddIntMacro(constants, ER_CREATE_DB_WITH_READ_LOCK) < 0) goto error;
#endif
#ifdef ER_WRONG_ARGUMENTS
    if (PyModule_AddIntMacro(constants, ER_WRONG_ARGUMENTS) < 0) goto error;
#endif
#ifdef ER_NO_PERMISSION_TO_CREATE_USER
    if (PyModule_AddIntMacro(constants, ER_NO_PERMISSION_TO_CREATE_USER) < 0) goto error;
#endif
#ifdef ER_UNION_TABLES_IN_DIFFERENT_DIR
    if (PyModule_AddIntMacro(constants, ER_UNION_TABLES_IN_DIFFERENT_DIR) < 0) goto error;
#endif
#ifdef ER_LOCK_DEADLOCK
    if (PyModule_AddIntMacro(constants, ER_LOCK_DEADLOCK) < 0) goto error;
#endif
#ifdef ER_TABLE_CANT_HANDLE_FT
    if (PyModule_AddIntMacro(constants, ER_TABLE_CANT_HANDLE_FT) < 0) goto error;
#endif
#ifdef ER_CANNOT_ADD_FOREIGN
    if (PyModule_AddIntMacro(constants, ER_CANNOT_ADD_FOREIGN) < 0) goto error;
#endif
#ifdef ER_NO_REFERENCED_ROW
    if (PyModule_AddIntMacro(constants, ER_NO_REFERENCED_ROW) < 0) goto error;
#endif
#ifdef ER_ROW_IS_REFERENCED
    if (PyModule_AddIntMacro(constants, ER_ROW_IS_REFERENCED) < 0) goto error;
#endif
#ifdef ER_CONNECT_TO_MASTER
    if (PyModule_AddIntMacro(constants, ER_CONNECT_TO_MASTER) < 0) goto error;
#endif
#ifdef ER_QUERY_ON_MASTER
    if (PyModule_AddIntMacro(constants, ER_QUERY_ON_MASTER) < 0) goto error;
#endif
#ifdef ER_ERROR_WHEN_EXECUTING_COMMAND
    if (PyModule_AddIntMacro(constants, ER_ERROR_WHEN_EXECUTING_COMMAND) < 0) goto error;
#endif
#ifdef ER_WRONG_USAGE
    if (PyModule_AddIntMacro(constants, ER_WRONG_USAGE) < 0) goto error;
#endif
#ifdef ER_WRONG_NUMBER_OF_COLUMNS_IN_SELECT
    if (PyModule_AddIntMacro(constants, ER_WRONG_NUMBER_OF_COLUMNS_IN_SELECT) < 0) goto error;
#endif
#ifdef ER_CANT_UPDATE_WITH_READLOCK
    if (PyModule_AddIntMacro(constants, ER_CANT_UPDATE_WITH_READLOCK) < 0) goto error;
#endif
#ifdef ER_MIXING_NOT_ALLOWED
    if (PyModule_AddIntMacro(constants, ER_MIXING_NOT_ALLOWED) < 0) goto error;
#endif
#ifdef ER_DUP_ARGUMENT
    if (PyModule_AddIntMacro(constants, ER_DUP_ARGUMENT) < 0) goto error;
#endif
#ifdef ER_USER_LIMIT_REACHED
    if (PyModule_AddIntMacro(constants, ER_USER_LIMIT_REACHED) < 0) goto error;
#endif
#ifdef ER_SPECIFIC_ACCESS_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_SPECIFIC_ACCESS_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_LOCAL_VARIABLE
    if (PyModule_AddIntMacro(constants, ER_LOCAL_VARIABLE) < 0) goto error;
#endif
#ifdef ER_GLOBAL_VARIABLE
    if (PyModule_AddIntMacro(constants, ER_GLOBAL_VARIABLE) < 0) goto error;
#endif
#ifdef ER_NO_DEFAULT
    if (PyModule_AddIntMacro(constants, ER_NO_DEFAULT) < 0) goto error;
#endif
#ifdef ER_WRONG_VALUE_FOR_VAR
    if (PyModule_AddIntMacro(constants, ER_WRONG_VALUE_FOR_VAR) < 0) goto error;
#endif
#ifdef ER_WRONG_TYPE_FOR_VAR
    if (PyModule_AddIntMacro(constants, ER_WRONG_TYPE_FOR_VAR) < 0) goto error;
#endif
#ifdef ER_VAR_CANT_BE_READ
    if (PyModule_AddIntMacro(constants, ER_VAR_CANT_BE_READ) < 0) goto error;
#endif
#ifdef ER_CANT_USE_OPTION_HERE
    if (PyModule_AddIntMacro(constants, ER_CANT_USE_OPTION_HERE) < 0) goto error;
#endif
#ifdef ER_NOT_SUPPORTED_YET
    if (PyModule_AddIntMacro(constants, ER_NOT_SUPPORTED_YET) < 0) goto error;
#endif
#ifdef ER_MASTER_FATAL_ERROR_READING_BINLOG
    if (PyModule_AddIntMacro(constants, ER_MASTER_FATAL_ERROR_READING_BINLOG) < 0) goto error;
#endif
#ifdef ER_SLAVE_IGNORED_TABLE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_IGNORED_TABLE) < 0) goto error;
#endif
#ifdef ER_INCORRECT_GLOBAL_LOCAL_VAR
    if (PyModule_AddIntMacro(constants, ER_INCORRECT_GLOBAL_LOCAL_VAR) < 0) goto error;
#endif
#ifdef ER_WRONG_FK_DEF
    if (PyModule_AddIntMacro(constants, ER_WRONG_FK_DEF) < 0) goto error;
#endif
#ifdef ER_KEY_REF_DO_NOT_MATCH_TABLE_REF
    if (PyModule_AddIntMacro(constants, ER_KEY_REF_DO_NOT_MATCH_TABLE_REF) < 0) goto error;
#endif
#ifdef ER_OPERAND_COLUMNS
    if (PyModule_AddIntMacro(constants, ER_OPERAND_COLUMNS) < 0) goto error;
#endif
#ifdef ER_SUBQUERY_NO_1_ROW
    if (PyModule_AddIntMacro(constants, ER_SUBQUERY_NO_1_ROW) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_STMT_HANDLER
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_STMT_HANDLER) < 0) goto error;
#endif
#ifdef ER_CORRUPT_HELP_DB
    if (PyModule_AddIntMacro(constants, ER_CORRUPT_HELP_DB) < 0) goto error;
#endif
#ifdef ER_CYCLIC_REFERENCE
    if (PyModule_AddIntMacro(constants, ER_CYCLIC_REFERENCE) < 0) goto error;
#endif
#ifdef ER_AUTO_CONVERT
    if (PyModule_AddIntMacro(constants, ER_AUTO_CONVERT) < 0) goto error;
#endif
#ifdef ER_ILLEGAL_REFERENCE
    if (PyModule_AddIntMacro(constants, ER_ILLEGAL_REFERENCE) < 0) goto error;
#endif
#ifdef ER_DERIVED_MUST_HAVE_ALIAS
    if (PyModule_AddIntMacro(constants, ER_DERIVED_MUST_HAVE_ALIAS) < 0) goto error;
#endif
#ifdef ER_SELECT_REDUCED
    if (PyModule_AddIntMacro(constants, ER_SELECT_REDUCED) < 0) goto error;
#endif
#ifdef ER_TABLENAME_NOT_ALLOWED_HERE
    if (PyModule_AddIntMacro(constants, ER_TABLENAME_NOT_ALLOWED_HERE) < 0) goto error;
#endif
#ifdef ER_NOT_SUPPORTED_AUTH_MODE
    if (PyModule_AddIntMacro(constants, ER_NOT_SUPPORTED_AUTH_MODE) < 0) goto error;
#endif
#ifdef ER_SPATIAL_CANT_HAVE_NULL
    if (PyModule_AddIntMacro(constants, ER_SPATIAL_CANT_HAVE_NULL) < 0) goto error;
#endif
#ifdef ER_COLLATION_CHARSET_MISMATCH
    if (PyModule_AddIntMacro(constants, ER_COLLATION_CHARSET_MISMATCH) < 0) goto error;
#endif
#ifdef ER_SLAVE_WAS_RUNNING
    if (PyModule_AddIntMacro(constants, ER_SLAVE_WAS_RUNNING) < 0) goto error;
#endif
#ifdef ER_SLAVE_WAS_NOT_RUNNING
    if (PyModule_AddIntMacro(constants, ER_SLAVE_WAS_NOT_RUNNING) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_FOR_UNCOMPRESS
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_FOR_UNCOMPRESS) < 0) goto error;
#endif
#ifdef ER_ZLIB_Z_MEM_ERROR
    if (PyModule_AddIntMacro(constants, ER_ZLIB_Z_MEM_ERROR) < 0) goto error;
#endif
#ifdef ER_ZLIB_Z_BUF_ERROR
    if (PyModule_AddIntMacro(constants, ER_ZLIB_Z_BUF_ERROR) < 0) goto error;
#endif
#ifdef ER_ZLIB_Z_DATA_ERROR
    if (PyModule_AddIntMacro(constants, ER_ZLIB_Z_DATA_ERROR) < 0) goto error;
#endif
#ifdef ER_CUT_VALUE_GROUP_CONCAT
    if (PyModule_AddIntMacro(constants, ER_CUT_VALUE_GROUP_CONCAT) < 0) goto error;
#endif
#ifdef ER_WARN_TOO_FEW_RECORDS
    if (PyModule_AddIntMacro(constants, ER_WARN_TOO_FEW_RECORDS) < 0) goto error;
#endif
#ifdef ER_WARN_TOO_MANY_RECORDS
    if (PyModule_AddIntMacro(constants, ER_WARN_TOO_MANY_RECORDS) < 0) goto error;
#endif
#ifdef ER_WARN_NULL_TO_NOTNULL
    if (PyModule_AddIntMacro(constants, ER_WARN_NULL_TO_NOTNULL) < 0) goto error;
#endif
#ifdef ER_WARN_DATA_OUT_OF_RANGE
    if (PyModule_AddIntMacro(constants, ER_WARN_DATA_OUT_OF_RANGE) < 0) goto error;
#endif
#ifdef WARN_DATA_TRUNCATED
    if (PyModule_AddIntMacro(constants, WARN_DATA_TRUNCATED) < 0) goto error;
#endif
#ifdef ER_WARN_USING_OTHER_HANDLER
    if (PyModule_AddIntMacro(constants, ER_WARN_USING_OTHER_HANDLER) < 0) goto error;
#endif
#ifdef ER_CANT_AGGREGATE_2COLLATIONS
    if (PyModule_AddIntMacro(constants, ER_CANT_AGGREGATE_2COLLATIONS) < 0) goto error;
#endif
#ifdef ER_DROP_USER
    if (PyModule_AddIntMacro(constants, ER_DROP_USER) < 0) goto error;
#endif
#ifdef ER_REVOKE_GRANTS
    if (PyModule_AddIntMacro(constants, ER_REVOKE_GRANTS) < 0) goto error;
#endif
#ifdef ER_CANT_AGGREGATE_3COLLATIONS
    if (PyModule_AddIntMacro(constants, ER_CANT_AGGREGATE_3COLLATIONS) < 0) goto error;
#endif
#ifdef ER_CANT_AGGREGATE_NCOLLATIONS
    if (PyModule_AddIntMacro(constants, ER_CANT_AGGREGATE_NCOLLATIONS) < 0) goto error;
#endif
#ifdef ER_VARIABLE_IS_NOT_STRUCT
    if (PyModule_AddIntMacro(constants, ER_VARIABLE_IS_NOT_STRUCT) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_COLLATION
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_COLLATION) < 0) goto error;
#endif
#ifdef ER_SLAVE_IGNORED_SSL_PARAMS
    if (PyModule_AddIntMacro(constants, ER_SLAVE_IGNORED_SSL_PARAMS) < 0) goto error;
#endif
#ifdef ER_SERVER_IS_IN_SECURE_AUTH_MODE
    if (PyModule_AddIntMacro(constants, ER_SERVER_IS_IN_SECURE_AUTH_MODE) < 0) goto error;
#endif
#ifdef ER_WARN_FIELD_RESOLVED
    if (PyModule_AddIntMacro(constants, ER_WARN_FIELD_RESOLVED) < 0) goto error;
#endif
#ifdef ER_BAD_SLAVE_UNTIL_COND
    if (PyModule_AddIntMacro(constants, ER_BAD_SLAVE_UNTIL_COND) < 0) goto error;
#endif
#ifdef ER_MISSING_SKIP_SLAVE
    if (PyModule_AddIntMacro(constants, ER_MISSING_SKIP_SLAVE) < 0) goto error;
#endif
#ifdef ER_UNTIL_COND_IGNORED
    if (PyModule_AddIntMacro(constants, ER_UNTIL_COND_IGNORED) < 0) goto error;
#endif
#ifdef ER_WRONG_NAME_FOR_INDEX
    if (PyModule_AddIntMacro(constants, ER_WRONG_NAME_FOR_INDEX) < 0) goto error;
#endif
#ifdef ER_WRONG_NAME_FOR_CATALOG
    if (PyModule_AddIntMacro(constants, ER_WRONG_NAME_FOR_CATALOG) < 0) goto error;
#endif
#ifdef ER_WARN_QC_RESIZE
    if (PyModule_AddIntMacro(constants, ER_WARN_QC_RESIZE) < 0) goto error;
#endif
#ifdef ER_BAD_FT_COLUMN
    if (PyModule_AddIntMacro(constants, ER_BAD_FT_COLUMN) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_KEY_CACHE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_KEY_CACHE) < 0) goto error;
#endif
#ifdef ER_WARN_HOSTNAME_WONT_WORK
    if (PyModule_AddIntMacro(constants, ER_WARN_HOSTNAME_WONT_WORK) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_STORAGE_ENGINE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_STORAGE_ENGINE) < 0) goto error;
#endif
#ifdef ER_WARN_DEPRECATED_SYNTAX
    if (PyModule_AddIntMacro(constants, ER_WARN_DEPRECATED_SYNTAX) < 0) goto error;
#endif
#ifdef ER_NON_UPDATABLE_TABLE
    if (PyModule_AddIntMacro(constants, ER_NON_UPDATABLE_TABLE) < 0) goto error;
#endif
#ifdef ER_FEATURE_DISABLED
    if (PyModule_AddIntMacro(constants, ER_FEATURE_DISABLED) < 0) goto error;
#endif
#ifdef ER_OPTION_PREVENTS_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_OPTION_PREVENTS_STATEMENT) < 0) goto error;
#endif
#ifdef ER_DUPLICATED_VALUE_IN_TYPE
    if (PyModule_AddIntMacro(constants, ER_DUPLICATED_VALUE_IN_TYPE) < 0) goto error;
#endif
#ifdef ER_TRUNCATED_WRONG_VALUE
    if (PyModule_AddIntMacro(constants, ER_TRUNCATED_WRONG_VALUE) < 0) goto error;
#endif
#ifdef ER_TOO_MUCH_AUTO_TIMESTAMP_COLS
    if (PyModule_AddIntMacro(constants, ER_TOO_MUCH_AUTO_TIMESTAMP_COLS) < 0) goto error;
#endif
#ifdef ER_INVALID_ON_UPDATE
    if (PyModule_AddIntMacro(constants, ER_INVALID_ON_UPDATE) < 0) goto error;
#endif
#ifdef ER_UNSUPPORTED_PS
    if (PyModule_AddIntMacro(constants, ER_UNSUPPORTED_PS) < 0) goto error;
#endif
#ifdef ER_GET_ERRMSG
    if (PyModule_AddIntMacro(constants, ER_GET_ERRMSG) < 0) goto error;
#endif
#ifdef ER_GET_TEMPORARY_ERRMSG
    if (PyModule_AddIntMacro(constants, ER_GET_TEMPORARY_ERRMSG) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_TIME_ZONE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_TIME_ZONE) < 0) goto error;
#endif
#ifdef ER_WARN_INVALID_TIMESTAMP
    if (PyModule_AddIntMacro(constants, ER_WARN_INVALID_TIMESTAMP) < 0) goto error;
#endif
#ifdef ER_INVALID_CHARACTER_STRING
    if (PyModule_AddIntMacro(constants, ER_INVALID_CHARACTER_STRING) < 0) goto error;
#endif
#ifdef ER_WARN_ALLOWED_PACKET_OVERFLOWED
    if (PyModule_AddIntMacro(constants, ER_WARN_ALLOWED_PACKET_OVERFLOWED) < 0) goto error;
#endif
#ifdef ER_CONFLICTING_DECLARATIONS
    if (PyModule_AddIntMacro(constants, ER_CONFLICTING_DECLARATIONS) < 0) goto error;
#endif
#ifdef ER_SP_NO_RECURSIVE_CREATE
    if (PyModule_AddIntMacro(constants, ER_SP_NO_RECURSIVE_CREATE) < 0) goto error;
#endif
#ifdef ER_SP_ALREADY_EXISTS
    if (PyModule_AddIntMacro(constants, ER_SP_ALREADY_EXISTS) < 0) goto error;
#endif
#ifdef ER_SP_DOES_NOT_EXIST
    if (PyModule_AddIntMacro(constants, ER_SP_DOES_NOT_EXIST) < 0) goto error;
#endif
#ifdef ER_SP_DROP_FAILED
    if (PyModule_AddIntMacro(constants, ER_SP_DROP_FAILED) < 0) goto error;
#endif
#ifdef ER_SP_STORE_FAILED
    if (PyModule_AddIntMacro(constants, ER_SP_STORE_FAILED) < 0) goto error;
#endif
#ifdef ER_SP_LILABEL_MISMATCH
    if (PyModule_AddIntMacro(constants, ER_SP_LILABEL_MISMATCH) < 0) goto error;
#endif
#ifdef ER_SP_LABEL_REDEFINE
    if (PyModule_AddIntMacro(constants, ER_SP_LABEL_REDEFINE) < 0) goto error;
#endif
#ifdef ER_SP_LABEL_MISMATCH
    if (PyModule_AddIntMacro(constants, ER_SP_LABEL_MISMATCH) < 0) goto error;
#endif
#ifdef ER_SP_UNINIT_VAR
    if (PyModule_AddIntMacro(constants, ER_SP_UNINIT_VAR) < 0) goto error;
#endif
#ifdef ER_SP_BADSELECT
    if (PyModule_AddIntMacro(constants, ER_SP_BADSELECT) < 0) goto error;
#endif
#ifdef ER_SP_BADRETURN
    if (PyModule_AddIntMacro(constants, ER_SP_BADRETURN) < 0) goto error;
#endif
#ifdef ER_SP_BADSTATEMENT
    if (PyModule_AddIntMacro(constants, ER_SP_BADSTATEMENT) < 0) goto error;
#endif
#ifdef ER_UPDATE_LOG_DEPRECATED_IGNORED
    if (PyModule_AddIntMacro(constants, ER_UPDATE_LOG_DEPRECATED_IGNORED) < 0) goto error;
#endif
#ifdef ER_UPDATE_LOG_DEPRECATED_TRANSLATED
    if (PyModule_AddIntMacro(constants, ER_UPDATE_LOG_DEPRECATED_TRANSLATED) < 0) goto error;
#endif
#ifdef ER_QUERY_INTERRUPTED
    if (PyModule_AddIntMacro(constants, ER_QUERY_INTERRUPTED) < 0) goto error;
#endif
#ifdef ER_SP_WRONG_NO_OF_ARGS
    if (PyModule_AddIntMacro(constants, ER_SP_WRONG_NO_OF_ARGS) < 0) goto error;
#endif
#ifdef ER_SP_COND_MISMATCH
    if (PyModule_AddIntMacro(constants, ER_SP_COND_MISMATCH) < 0) goto error;
#endif
#ifdef ER_SP_NORETURN
    if (PyModule_AddIntMacro(constants, ER_SP_NORETURN) < 0) goto error;
#endif
#ifdef ER_SP_NORETURNEND
    if (PyModule_AddIntMacro(constants, ER_SP_NORETURNEND) < 0) goto error;
#endif
#ifdef ER_SP_BAD_CURSOR_QUERY
    if (PyModule_AddIntMacro(constants, ER_SP_BAD_CURSOR_QUERY) < 0) goto error;
#endif
#ifdef ER_SP_BAD_CURSOR_SELECT
    if (PyModule_AddIntMacro(constants, ER_SP_BAD_CURSOR_SELECT) < 0) goto error;
#endif
#ifdef ER_SP_CURSOR_MISMATCH
    if (PyModule_AddIntMacro(constants, ER_SP_CURSOR_MISMATCH) < 0) goto error;
#endif
#ifdef ER_SP_CURSOR_ALREADY_OPEN
    if (PyModule_AddIntMacro(constants, ER_SP_CURSOR_ALREADY_OPEN) < 0) goto error;
#endif
#ifdef ER_SP_CURSOR_NOT_OPEN
    if (PyModule_AddIntMacro(constants, ER_SP_CURSOR_NOT_OPEN) < 0) goto error;
#endif
#ifdef ER_SP_UNDECLARED_VAR
    if (PyModule_AddIntMacro(constants, ER_SP_UNDECLARED_VAR) < 0) goto error;
#endif
#ifdef ER_SP_WRONG_NO_OF_FETCH_ARGS
    if (PyModule_AddIntMacro(constants, ER_SP_WRONG_NO_OF_FETCH_ARGS) < 0) goto error;
#endif
#ifdef ER_SP_FETCH_NO_DATA
    if (PyModule_AddIntMacro(constants, ER_SP_FETCH_NO_DATA) < 0) goto error;
#endif
#ifdef ER_SP_DUP_PARAM
    if (PyModule_AddIntMacro(constants, ER_SP_DUP_PARAM) < 0) goto error;
#endif
#ifdef ER_SP_DUP_VAR
    if (PyModule_AddIntMacro(constants, ER_SP_DUP_VAR) < 0) goto error;
#endif
#ifdef ER_SP_DUP_COND
    if (PyModule_AddIntMacro(constants, ER_SP_DUP_COND) < 0) goto error;
#endif
#ifdef ER_SP_DUP_CURS
    if (PyModule_AddIntMacro(constants, ER_SP_DUP_CURS) < 0) goto error;
#endif
#ifdef ER_SP_CANT_ALTER
    if (PyModule_AddIntMacro(constants, ER_SP_CANT_ALTER) < 0) goto error;
#endif
#ifdef ER_SP_SUBSELECT_NYI
    if (PyModule_AddIntMacro(constants, ER_SP_SUBSELECT_NYI) < 0) goto error;
#endif
#ifdef ER_STMT_NOT_ALLOWED_IN_SF_OR_TRG
    if (PyModule_AddIntMacro(constants, ER_STMT_NOT_ALLOWED_IN_SF_OR_TRG) < 0) goto error;
#endif
#ifdef ER_SP_VARCOND_AFTER_CURSHNDLR
    if (PyModule_AddIntMacro(constants, ER_SP_VARCOND_AFTER_CURSHNDLR) < 0) goto error;
#endif
#ifdef ER_SP_CURSOR_AFTER_HANDLER
    if (PyModule_AddIntMacro(constants, ER_SP_CURSOR_AFTER_HANDLER) < 0) goto error;
#endif
#ifdef ER_SP_CASE_NOT_FOUND
    if (PyModule_AddIntMacro(constants, ER_SP_CASE_NOT_FOUND) < 0) goto error;
#endif
#ifdef ER_FPARSER_TOO_BIG_FILE
    if (PyModule_AddIntMacro(constants, ER_FPARSER_TOO_BIG_FILE) < 0) goto error;
#endif
#ifdef ER_FPARSER_BAD_HEADER
    if (PyModule_AddIntMacro(constants, ER_FPARSER_BAD_HEADER) < 0) goto error;
#endif
#ifdef ER_FPARSER_EOF_IN_COMMENT
    if (PyModule_AddIntMacro(constants, ER_FPARSER_EOF_IN_COMMENT) < 0) goto error;
#endif
#ifdef ER_FPARSER_ERROR_IN_PARAMETER
    if (PyModule_AddIntMacro(constants, ER_FPARSER_ERROR_IN_PARAMETER) < 0) goto error;
#endif
#ifdef ER_FPARSER_EOF_IN_UNKNOWN_PARAMETER
    if (PyModule_AddIntMacro(constants, ER_FPARSER_EOF_IN_UNKNOWN_PARAMETER) < 0) goto error;
#endif
#ifdef ER_VIEW_NO_EXPLAIN
    if (PyModule_AddIntMacro(constants, ER_VIEW_NO_EXPLAIN) < 0) goto error;
#endif
#ifdef ER_FRM_UNKNOWN_TYPE
    if (PyModule_AddIntMacro(constants, ER_FRM_UNKNOWN_TYPE) < 0) goto error;
#endif
#ifdef ER_WRONG_OBJECT
    if (PyModule_AddIntMacro(constants, ER_WRONG_OBJECT) < 0) goto error;
#endif
#ifdef ER_NONUPDATEABLE_COLUMN
    if (PyModule_AddIntMacro(constants, ER_NONUPDATEABLE_COLUMN) < 0) goto error;
#endif
#ifdef ER_VIEW_SELECT_DERIVED
    if (PyModule_AddIntMacro(constants, ER_VIEW_SELECT_DERIVED) < 0) goto error;
#endif
#ifdef ER_VIEW_SELECT_CLAUSE
    if (PyModule_AddIntMacro(constants, ER_VIEW_SELECT_CLAUSE) < 0) goto error;
#endif
#ifdef ER_VIEW_SELECT_VARIABLE
    if (PyModule_AddIntMacro(constants, ER_VIEW_SELECT_VARIABLE) < 0) goto error;
#endif
#ifdef ER_VIEW_SELECT_TMPTABLE
    if (PyModule_AddIntMacro(constants, ER_VIEW_SELECT_TMPTABLE) < 0) goto error;
#endif
#ifdef ER_VIEW_WRONG_LIST
    if (PyModule_AddIntMacro(constants, ER_VIEW_WRONG_LIST) < 0) goto error;
#endif
#ifdef ER_WARN_VIEW_MERGE
    if (PyModule_AddIntMacro(constants, ER_WARN_VIEW_MERGE) < 0) goto error;
#endif
#ifdef ER_WARN_VIEW_WITHOUT_KEY
    if (PyModule_AddIntMacro(constants, ER_WARN_VIEW_WITHOUT_KEY) < 0) goto error;
#endif
#ifdef ER_VIEW_INVALID
    if (PyModule_AddIntMacro(constants, ER_VIEW_INVALID) < 0) goto error;
#endif
#ifdef ER_SP_NO_DROP_SP
    if (PyModule_AddIntMacro(constants, ER_SP_NO_DROP_SP) < 0) goto error;
#endif
#ifdef ER_SP_GOTO_IN_HNDLR
    if (PyModule_AddIntMacro(constants, ER_SP_GOTO_IN_HNDLR) < 0) goto error;
#endif
#ifdef ER_TRG_ALREADY_EXISTS
    if (PyModule_AddIntMacro(constants, ER_TRG_ALREADY_EXISTS) < 0) goto error;
#endif
#ifdef ER_TRG_DOES_NOT_EXIST
    if (PyModule_AddIntMacro(constants, ER_TRG_DOES_NOT_EXIST) < 0) goto error;
#endif
#ifdef ER_TRG_ON_VIEW_OR_TEMP_TABLE
    if (PyModule_AddIntMacro(constants, ER_TRG_ON_VIEW_OR_TEMP_TABLE) < 0) goto error;
#endif
#ifdef ER_TRG_CANT_CHANGE_ROW
    if (PyModule_AddIntMacro(constants, ER_TRG_CANT_CHANGE_ROW) < 0) goto error;
#endif
#ifdef ER_TRG_NO_SUCH_ROW_IN_TRG
    if (PyModule_AddIntMacro(constants, ER_TRG_NO_SUCH_ROW_IN_TRG) < 0) goto error;
#endif
#ifdef ER_NO_DEFAULT_FOR_FIELD
    if (PyModule_AddIntMacro(constants, ER_NO_DEFAULT_FOR_FIELD) < 0) goto error;
#endif
#ifdef ER_DIVISION_BY_ZERO
    if (PyModule_AddIntMacro(constants, ER_DIVISION_BY_ZERO) < 0) goto error;
#endif
#ifdef ER_TRUNCATED_WRONG_VALUE_FOR_FIELD
    if (PyModule_AddIntMacro(constants, ER_TRUNCATED_WRONG_VALUE_FOR_FIELD) < 0) goto error;
#endif
#ifdef ER_ILLEGAL_VALUE_FOR_TYPE
    if (PyModule_AddIntMacro(constants, ER_ILLEGAL_VALUE_FOR_TYPE) < 0) goto error;
#endif
#ifdef ER_VIEW_NONUPD_CHECK
    if (PyModule_AddIntMacro(constants, ER_VIEW_NONUPD_CHECK) < 0) goto error;
#endif
#ifdef ER_VIEW_CHECK_FAILED
    if (PyModule_AddIntMacro(constants, ER_VIEW_CHECK_FAILED) < 0) goto error;
#endif
#ifdef ER_PROCACCESS_DENIED_ERROR
    if (PyModule_AddIntMacro(constants, ER_PROCACCESS_DENIED_ERROR) < 0) goto error;
#endif
#ifdef ER_RELAY_LOG_FAIL
    if (PyModule_AddIntMacro(constants, ER_RELAY_LOG_FAIL) < 0) goto error;
#endif
#ifdef ER_PASSWD_LENGTH
    if (PyModule_AddIntMacro(constants, ER_PASSWD_LENGTH) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_TARGET_BINLOG
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_TARGET_BINLOG) < 0) goto error;
#endif
#ifdef ER_IO_ERR_LOG_INDEX_READ
    if (PyModule_AddIntMacro(constants, ER_IO_ERR_LOG_INDEX_READ) < 0) goto error;
#endif
#ifdef ER_BINLOG_PURGE_PROHIBITED
    if (PyModule_AddIntMacro(constants, ER_BINLOG_PURGE_PROHIBITED) < 0) goto error;
#endif
#ifdef ER_FSEEK_FAIL
    if (PyModule_AddIntMacro(constants, ER_FSEEK_FAIL) < 0) goto error;
#endif
#ifdef ER_BINLOG_PURGE_FATAL_ERR
    if (PyModule_AddIntMacro(constants, ER_BINLOG_PURGE_FATAL_ERR) < 0) goto error;
#endif
#ifdef ER_LOG_IN_USE
    if (PyModule_AddIntMacro(constants, ER_LOG_IN_USE) < 0) goto error;
#endif
#ifdef ER_LOG_PURGE_UNKNOWN_ERR
    if (PyModule_AddIntMacro(constants, ER_LOG_PURGE_UNKNOWN_ERR) < 0) goto error;
#endif
#ifdef ER_RELAY_LOG_INIT
    if (PyModule_AddIntMacro(constants, ER_RELAY_LOG_INIT) < 0) goto error;
#endif
#ifdef ER_NO_BINARY_LOGGING
    if (PyModule_AddIntMacro(constants, ER_NO_BINARY_LOGGING) < 0) goto error;
#endif
#ifdef ER_RESERVED_SYNTAX
    if (PyModule_AddIntMacro(constants, ER_RESERVED_SYNTAX) < 0) goto error;
#endif
#ifdef ER_WSAS_FAILED
    if (PyModule_AddIntMacro(constants, ER_WSAS_FAILED) < 0) goto error;
#endif
#ifdef ER_DIFF_GROUPS_PROC
    if (PyModule_AddIntMacro(constants, ER_DIFF_GROUPS_PROC) < 0) goto error;
#endif
#ifdef ER_NO_GROUP_FOR_PROC
    if (PyModule_AddIntMacro(constants, ER_NO_GROUP_FOR_PROC) < 0) goto error;
#endif
#ifdef ER_ORDER_WITH_PROC
    if (PyModule_AddIntMacro(constants, ER_ORDER_WITH_PROC) < 0) goto error;
#endif
#ifdef ER_LOGGING_PROHIBIT_CHANGING_OF
    if (PyModule_AddIntMacro(constants, ER_LOGGING_PROHIBIT_CHANGING_OF) < 0) goto error;
#endif
#ifdef ER_NO_FILE_MAPPING
    if (PyModule_AddIntMacro(constants, ER_NO_FILE_MAPPING) < 0) goto error;
#endif
#ifdef ER_WRONG_MAGIC
    if (PyModule_AddIntMacro(constants, ER_WRONG_MAGIC) < 0) goto error;
#endif
#ifdef ER_PS_MANY_PARAM
    if (PyModule_AddIntMacro(constants, ER_PS_MANY_PARAM) < 0) goto error;
#endif
#ifdef ER_KEY_PART_0
    if (PyModule_AddIntMacro(constants, ER_KEY_PART_0) < 0) goto error;
#endif
#ifdef ER_VIEW_CHECKSUM
    if (PyModule_AddIntMacro(constants, ER_VIEW_CHECKSUM) < 0) goto error;
#endif
#ifdef ER_VIEW_MULTIUPDATE
    if (PyModule_AddIntMacro(constants, ER_VIEW_MULTIUPDATE) < 0) goto error;
#endif
#ifdef ER_VIEW_NO_INSERT_FIELD_LIST
    if (PyModule_AddIntMacro(constants, ER_VIEW_NO_INSERT_FIELD_LIST) < 0) goto error;
#endif
#ifdef ER_VIEW_DELETE_MERGE_VIEW
    if (PyModule_AddIntMacro(constants, ER_VIEW_DELETE_MERGE_VIEW) < 0) goto error;
#endif
#ifdef ER_CANNOT_USER
    if (PyModule_AddIntMacro(constants, ER_CANNOT_USER) < 0) goto error;
#endif
#ifdef ER_XAER_NOTA
    if (PyModule_AddIntMacro(constants, ER_XAER_NOTA) < 0) goto error;
#endif
#ifdef ER_XAER_INVAL
    if (PyModule_AddIntMacro(constants, ER_XAER_INVAL) < 0) goto error;
#endif
#ifdef ER_XAER_RMFAIL
    if (PyModule_AddIntMacro(constants, ER_XAER_RMFAIL) < 0) goto error;
#endif
#ifdef ER_XAER_OUTSIDE
    if (PyModule_AddIntMacro(constants, ER_XAER_OUTSIDE) < 0) goto error;
#endif
#ifdef ER_XAER_RMERR
    if (PyModule_AddIntMacro(constants, ER_XAER_RMERR) < 0) goto error;
#endif
#ifdef ER_XA_RBROLLBACK
    if (PyModule_AddIntMacro(constants, ER_XA_RBROLLBACK) < 0) goto error;
#endif
#ifdef ER_NONEXISTING_PROC_GRANT
    if (PyModule_AddIntMacro(constants, ER_NONEXISTING_PROC_GRANT) < 0) goto error;
#endif
#ifdef ER_PROC_AUTO_GRANT_FAIL
    if (PyModule_AddIntMacro(constants, ER_PROC_AUTO_GRANT_FAIL) < 0) goto error;
#endif
#ifdef ER_PROC_AUTO_REVOKE_FAIL
    if (PyModule_AddIntMacro(constants, ER_PROC_AUTO_REVOKE_FAIL) < 0) goto error;
#endif
#ifdef ER_DATA_TOO_LONG
    if (PyModule_AddIntMacro(constants, ER_DATA_TOO_LONG) < 0) goto error;
#endif
#ifdef ER_SP_BAD_SQLSTATE
    if (PyModule_AddIntMacro(constants, ER_SP_BAD_SQLSTATE) < 0) goto error;
#endif
#ifdef ER_STARTUP
    if (PyModule_AddIntMacro(constants, ER_STARTUP) < 0) goto error;
#endif
#ifdef ER_LOAD_FROM_FIXED_SIZE_ROWS_TO_VAR
    if (PyModule_AddIntMacro(constants, ER_LOAD_FROM_FIXED_SIZE_ROWS_TO_VAR) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_USER_WITH_GRANT
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_USER_WITH_GRANT) < 0) goto error;
#endif
#ifdef ER_WRONG_VALUE_FOR_TYPE
    if (PyModule_AddIntMacro(constants, ER_WRONG_VALUE_FOR_TYPE) < 0) goto error;
#endif
#ifdef ER_TABLE_DEF_CHANGED
    if (PyModule_AddIntMacro(constants, ER_TABLE_DEF_CHANGED) < 0) goto error;
#endif
#ifdef ER_SP_DUP_HANDLER
    if (PyModule_AddIntMacro(constants, ER_SP_DUP_HANDLER) < 0) goto error;
#endif
#ifdef ER_SP_NOT_VAR_ARG
    if (PyModule_AddIntMacro(constants, ER_SP_NOT_VAR_ARG) < 0) goto error;
#endif
#ifdef ER_SP_NO_RETSET
    if (PyModule_AddIntMacro(constants, ER_SP_NO_RETSET) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_GEOMETRY_OBJECT
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_GEOMETRY_OBJECT) < 0) goto error;
#endif
#ifdef ER_FAILED_ROUTINE_BREAK_BINLOG
    if (PyModule_AddIntMacro(constants, ER_FAILED_ROUTINE_BREAK_BINLOG) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_ROUTINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_ROUTINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_CREATE_ROUTINE_NEED_SUPER
    if (PyModule_AddIntMacro(constants, ER_BINLOG_CREATE_ROUTINE_NEED_SUPER) < 0) goto error;
#endif
#ifdef ER_EXEC_STMT_WITH_OPEN_CURSOR
    if (PyModule_AddIntMacro(constants, ER_EXEC_STMT_WITH_OPEN_CURSOR) < 0) goto error;
#endif
#ifdef ER_STMT_HAS_NO_OPEN_CURSOR
    if (PyModule_AddIntMacro(constants, ER_STMT_HAS_NO_OPEN_CURSOR) < 0) goto error;
#endif
#ifdef ER_COMMIT_NOT_ALLOWED_IN_SF_OR_TRG
    if (PyModule_AddIntMacro(constants, ER_COMMIT_NOT_ALLOWED_IN_SF_OR_TRG) < 0) goto error;
#endif
#ifdef ER_NO_DEFAULT_FOR_VIEW_FIELD
    if (PyModule_AddIntMacro(constants, ER_NO_DEFAULT_FOR_VIEW_FIELD) < 0) goto error;
#endif
#ifdef ER_SP_NO_RECURSION
    if (PyModule_AddIntMacro(constants, ER_SP_NO_RECURSION) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_SCALE
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_SCALE) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_PRECISION
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_PRECISION) < 0) goto error;
#endif
#ifdef ER_M_BIGGER_THAN_D
    if (PyModule_AddIntMacro(constants, ER_M_BIGGER_THAN_D) < 0) goto error;
#endif
#ifdef ER_WRONG_LOCK_OF_SYSTEM_TABLE
    if (PyModule_AddIntMacro(constants, ER_WRONG_LOCK_OF_SYSTEM_TABLE) < 0) goto error;
#endif
#ifdef ER_CONNECT_TO_FOREIGN_DATA_SOURCE
    if (PyModule_AddIntMacro(constants, ER_CONNECT_TO_FOREIGN_DATA_SOURCE) < 0) goto error;
#endif
#ifdef ER_QUERY_ON_FOREIGN_DATA_SOURCE
    if (PyModule_AddIntMacro(constants, ER_QUERY_ON_FOREIGN_DATA_SOURCE) < 0) goto error;
#endif
#ifdef ER_FOREIGN_DATA_SOURCE_DOESNT_EXIST
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_DATA_SOURCE_DOESNT_EXIST) < 0) goto error;
#endif
#ifdef ER_FOREIGN_DATA_STRING_INVALID_CANT_CREATE
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_DATA_STRING_INVALID_CANT_CREATE) < 0) goto error;
#endif
#ifdef ER_FOREIGN_DATA_STRING_INVALID
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_DATA_STRING_INVALID) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_FEDERATED_TABLE
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_FEDERATED_TABLE) < 0) goto error;
#endif
#ifdef ER_TRG_IN_WRONG_SCHEMA
    if (PyModule_AddIntMacro(constants, ER_TRG_IN_WRONG_SCHEMA) < 0) goto error;
#endif
#ifdef ER_STACK_OVERRUN_NEED_MORE
    if (PyModule_AddIntMacro(constants, ER_STACK_OVERRUN_NEED_MORE) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_BODY
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_BODY) < 0) goto error;
#endif
#ifdef ER_WARN_CANT_DROP_DEFAULT_KEYCACHE
    if (PyModule_AddIntMacro(constants, ER_WARN_CANT_DROP_DEFAULT_KEYCACHE) < 0) goto error;
#endif
#ifdef ER_TOO_BIG_DISPLAYWIDTH
    if (PyModule_AddIntMacro(constants, ER_TOO_BIG_DISPLAYWIDTH) < 0) goto error;
#endif
#ifdef ER_XAER_DUPID
    if (PyModule_AddIntMacro(constants, ER_XAER_DUPID) < 0) goto error;
#endif
#ifdef ER_DATETIME_FUNCTION_OVERFLOW
    if (PyModule_AddIntMacro(constants, ER_DATETIME_FUNCTION_OVERFLOW) < 0) goto error;
#endif
#ifdef ER_CANT_UPDATE_USED_TABLE_IN_SF_OR_TRG
    if (PyModule_AddIntMacro(constants, ER_CANT_UPDATE_USED_TABLE_IN_SF_OR_TRG) < 0) goto error;
#endif
#ifdef ER_VIEW_PREVENT_UPDATE
    if (PyModule_AddIntMacro(constants, ER_VIEW_PREVENT_UPDATE) < 0) goto error;
#endif
#ifdef ER_PS_NO_RECURSION
    if (PyModule_AddIntMacro(constants, ER_PS_NO_RECURSION) < 0) goto error;
#endif
#ifdef ER_SP_CANT_SET_AUTOCOMMIT
    if (PyModule_AddIntMacro(constants, ER_SP_CANT_SET_AUTOCOMMIT) < 0) goto error;
#endif
#ifdef ER_MALFORMED_DEFINER
    if (PyModule_AddIntMacro(constants, ER_MALFORMED_DEFINER) < 0) goto error;
#endif
#ifdef ER_VIEW_FRM_NO_USER
    if (PyModule_AddIntMacro(constants, ER_VIEW_FRM_NO_USER) < 0) goto error;
#endif
#ifdef ER_VIEW_OTHER_USER
    if (PyModule_AddIntMacro(constants, ER_VIEW_OTHER_USER) < 0) goto error;
#endif
#ifdef ER_NO_SUCH_USER
    if (PyModule_AddIntMacro(constants, ER_NO_SUCH_USER) < 0) goto error;
#endif
#ifdef ER_FORBID_SCHEMA_CHANGE
    if (PyModule_AddIntMacro(constants, ER_FORBID_SCHEMA_CHANGE) < 0) goto error;
#endif
#ifdef ER_ROW_IS_REFERENCED_2
    if (PyModule_AddIntMacro(constants, ER_ROW_IS_REFERENCED_2) < 0) goto error;
#endif
#ifdef ER_NO_REFERENCED_ROW_2
    if (PyModule_AddIntMacro(constants, ER_NO_REFERENCED_ROW_2) < 0) goto error;
#endif
#ifdef ER_SP_BAD_VAR_SHADOW
    if (PyModule_AddIntMacro(constants, ER_SP_BAD_VAR_SHADOW) < 0) goto error;
#endif
#ifdef ER_TRG_NO_DEFINER
    if (PyModule_AddIntMacro(constants, ER_TRG_NO_DEFINER) < 0) goto error;
#endif
#ifdef ER_OLD_FILE_FORMAT
    if (PyModule_AddIntMacro(constants, ER_OLD_FILE_FORMAT) < 0) goto error;
#endif
#ifdef ER_SP_RECURSION_LIMIT
    if (PyModule_AddIntMacro(constants, ER_SP_RECURSION_LIMIT) < 0) goto error;
#endif
#ifdef ER_SP_PROC_TABLE_CORRUPT
    if (PyModule_AddIntMacro(constants, ER_SP_PROC_TABLE_CORRUPT) < 0) goto error;
#endif
#ifdef ER_SP_WRONG_NAME
    if (PyModule_AddIntMacro(constants, ER_SP_WRONG_NAME) < 0) goto error;
#endif
#ifdef ER_TABLE_NEEDS_UPGRADE
    if (PyModule_AddIntMacro(constants, ER_TABLE_NEEDS_UPGRADE) < 0) goto error;
#endif
#ifdef ER_SP_NO_AGGREGATE
    if (PyModule_AddIntMacro(constants, ER_SP_NO_AGGREGATE) < 0) goto error;
#endif
#ifdef ER_MAX_PREPARED_STMT_COUNT_REACHED
    if (PyModule_AddIntMacro(constants, ER_MAX_PREPARED_STMT_COUNT_REACHED) < 0) goto error;
#endif
#ifdef ER_VIEW_RECURSIVE
    if (PyModule_AddIntMacro(constants, ER_VIEW_RECURSIVE) < 0) goto error;
#endif
#ifdef ER_NON_GROUPING_FIELD_USED
    if (PyModule_AddIntMacro(constants, ER_NON_GROUPING_FIELD_USED) < 0) goto error;
#endif
#ifdef ER_TABLE_CANT_HANDLE_SPKEYS
    if (PyModule_AddIntMacro(constants, ER_TABLE_CANT_HANDLE_SPKEYS) < 0) goto error;
#endif
#ifdef ER_NO_TRIGGERS_ON_SYSTEM_SCHEMA
    if (PyModule_AddIntMacro(constants, ER_NO_TRIGGERS_ON_SYSTEM_SCHEMA) < 0) goto error;
#endif
#ifdef ER_REMOVED_SPACES
    if (PyModule_AddIntMacro(constants, ER_REMOVED_SPACES) < 0) goto error;
#endif
#ifdef ER_AUTOINC_READ_FAILED
    if (PyModule_AddIntMacro(constants, ER_AUTOINC_READ_FAILED) < 0) goto error;
#endif
#ifdef ER_USERNAME
    if (PyModule_AddIntMacro(constants, ER_USERNAME) < 0) goto error;
#endif
#ifdef ER_HOSTNAME
    if (PyModule_AddIntMacro(constants, ER_HOSTNAME) < 0) goto error;
#endif
#ifdef ER_WRONG_STRING_LENGTH
    if (PyModule_AddIntMacro(constants, ER_WRONG_STRING_LENGTH) < 0) goto error;
#endif
#ifdef ER_NON_INSERTABLE_TABLE
    if (PyModule_AddIntMacro(constants, ER_NON_INSERTABLE_TABLE) < 0) goto error;
#endif
#ifdef ER_ADMIN_WRONG_MRG_TABLE
    if (PyModule_AddIntMacro(constants, ER_ADMIN_WRONG_MRG_TABLE) < 0) goto error;
#endif
#ifdef ER_TOO_HIGH_LEVEL_OF_NESTING_FOR_SELECT
    if (PyModule_AddIntMacro(constants, ER_TOO_HIGH_LEVEL_OF_NESTING_FOR_SELECT) < 0) goto error;
#endif
#ifdef ER_NAME_BECOMES_EMPTY
    if (PyModule_AddIntMacro(constants, ER_NAME_BECOMES_EMPTY) < 0) goto error;
#endif
#ifdef ER_AMBIGUOUS_FIELD_TERM
    if (PyModule_AddIntMacro(constants, ER_AMBIGUOUS_FIELD_TERM) < 0) goto error;
#endif
#ifdef ER_FOREIGN_SERVER_EXISTS
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_SERVER_EXISTS) < 0) goto error;
#endif
#ifdef ER_FOREIGN_SERVER_DOESNT_EXIST
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_SERVER_DOESNT_EXIST) < 0) goto error;
#endif
#ifdef ER_ILLEGAL_HA_CREATE_OPTION
    if (PyModule_AddIntMacro(constants, ER_ILLEGAL_HA_CREATE_OPTION) < 0) goto error;
#endif
#ifdef ER_PARTITION_REQUIRES_VALUES_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_REQUIRES_VALUES_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_WRONG_VALUES_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_WRONG_VALUES_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_MAXVALUE_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_MAXVALUE_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_SUBPARTITION_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_SUBPARTITION_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_SUBPART_MIX_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_SUBPART_MIX_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_WRONG_NO_PART_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_WRONG_NO_PART_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_WRONG_NO_SUBPART_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_WRONG_NO_SUBPART_ERROR) < 0) goto error;
#endif
#ifdef ER_WRONG_EXPR_IN_PARTITION_FUNC_ERROR
    if (PyModule_AddIntMacro(constants, ER_WRONG_EXPR_IN_PARTITION_FUNC_ERROR) < 0) goto error;
#endif
#ifdef ER_NO_CONST_EXPR_IN_RANGE_OR_LIST_ERROR
    if (PyModule_AddIntMacro(constants, ER_NO_CONST_EXPR_IN_RANGE_OR_LIST_ERROR) < 0) goto error;
#endif
#ifdef ER_FIELD_NOT_FOUND_PART_ERROR
    if (PyModule_AddIntMacro(constants, ER_FIELD_NOT_FOUND_PART_ERROR) < 0) goto error;
#endif
#ifdef ER_LIST_OF_FIELDS_ONLY_IN_HASH_ERROR
    if (PyModule_AddIntMacro(constants, ER_LIST_OF_FIELDS_ONLY_IN_HASH_ERROR) < 0) goto error;
#endif
#ifdef ER_INCONSISTENT_PARTITION_INFO_ERROR
    if (PyModule_AddIntMacro(constants, ER_INCONSISTENT_PARTITION_INFO_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_FUNC_NOT_ALLOWED_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_FUNC_NOT_ALLOWED_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITIONS_MUST_BE_DEFINED_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITIONS_MUST_BE_DEFINED_ERROR) < 0) goto error;
#endif
#ifdef ER_RANGE_NOT_INCREASING_ERROR
    if (PyModule_AddIntMacro(constants, ER_RANGE_NOT_INCREASING_ERROR) < 0) goto error;
#endif
#ifdef ER_INCONSISTENT_TYPE_OF_FUNCTIONS_ERROR
    if (PyModule_AddIntMacro(constants, ER_INCONSISTENT_TYPE_OF_FUNCTIONS_ERROR) < 0) goto error;
#endif
#ifdef ER_MULTIPLE_DEF_CONST_IN_LIST_PART_ERROR
    if (PyModule_AddIntMacro(constants, ER_MULTIPLE_DEF_CONST_IN_LIST_PART_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_ENTRY_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_ENTRY_ERROR) < 0) goto error;
#endif
#ifdef ER_MIX_HANDLER_ERROR
    if (PyModule_AddIntMacro(constants, ER_MIX_HANDLER_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_NOT_DEFINED_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_NOT_DEFINED_ERROR) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_PARTITIONS_ERROR
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_PARTITIONS_ERROR) < 0) goto error;
#endif
#ifdef ER_SUBPARTITION_ERROR
    if (PyModule_AddIntMacro(constants, ER_SUBPARTITION_ERROR) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_HANDLER_FILE
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_HANDLER_FILE) < 0) goto error;
#endif
#ifdef ER_BLOB_FIELD_IN_PART_FUNC_ERROR
    if (PyModule_AddIntMacro(constants, ER_BLOB_FIELD_IN_PART_FUNC_ERROR) < 0) goto error;
#endif
#ifdef ER_UNIQUE_KEY_NEED_ALL_FIELDS_IN_PF
    if (PyModule_AddIntMacro(constants, ER_UNIQUE_KEY_NEED_ALL_FIELDS_IN_PF) < 0) goto error;
#endif
#ifdef ER_NO_PARTS_ERROR
    if (PyModule_AddIntMacro(constants, ER_NO_PARTS_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_MGMT_ON_NONPARTITIONED
    if (PyModule_AddIntMacro(constants, ER_PARTITION_MGMT_ON_NONPARTITIONED) < 0) goto error;
#endif
#ifdef ER_FOREIGN_KEY_ON_PARTITIONED
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_KEY_ON_PARTITIONED) < 0) goto error;
#endif
#ifdef ER_DROP_PARTITION_NON_EXISTENT
    if (PyModule_AddIntMacro(constants, ER_DROP_PARTITION_NON_EXISTENT) < 0) goto error;
#endif
#ifdef ER_DROP_LAST_PARTITION
    if (PyModule_AddIntMacro(constants, ER_DROP_LAST_PARTITION) < 0) goto error;
#endif
#ifdef ER_COALESCE_ONLY_ON_HASH_PARTITION
    if (PyModule_AddIntMacro(constants, ER_COALESCE_ONLY_ON_HASH_PARTITION) < 0) goto error;
#endif
#ifdef ER_REORG_HASH_ONLY_ON_SAME_NO
    if (PyModule_AddIntMacro(constants, ER_REORG_HASH_ONLY_ON_SAME_NO) < 0) goto error;
#endif
#ifdef ER_REORG_NO_PARAM_ERROR
    if (PyModule_AddIntMacro(constants, ER_REORG_NO_PARAM_ERROR) < 0) goto error;
#endif
#ifdef ER_ONLY_ON_RANGE_LIST_PARTITION
    if (PyModule_AddIntMacro(constants, ER_ONLY_ON_RANGE_LIST_PARTITION) < 0) goto error;
#endif
#ifdef ER_ADD_PARTITION_SUBPART_ERROR
    if (PyModule_AddIntMacro(constants, ER_ADD_PARTITION_SUBPART_ERROR) < 0) goto error;
#endif
#ifdef ER_ADD_PARTITION_NO_NEW_PARTITION
    if (PyModule_AddIntMacro(constants, ER_ADD_PARTITION_NO_NEW_PARTITION) < 0) goto error;
#endif
#ifdef ER_COALESCE_PARTITION_NO_PARTITION
    if (PyModule_AddIntMacro(constants, ER_COALESCE_PARTITION_NO_PARTITION) < 0) goto error;
#endif
#ifdef ER_REORG_PARTITION_NOT_EXIST
    if (PyModule_AddIntMacro(constants, ER_REORG_PARTITION_NOT_EXIST) < 0) goto error;
#endif
#ifdef ER_SAME_NAME_PARTITION
    if (PyModule_AddIntMacro(constants, ER_SAME_NAME_PARTITION) < 0) goto error;
#endif
#ifdef ER_NO_BINLOG_ERROR
    if (PyModule_AddIntMacro(constants, ER_NO_BINLOG_ERROR) < 0) goto error;
#endif
#ifdef ER_CONSECUTIVE_REORG_PARTITIONS
    if (PyModule_AddIntMacro(constants, ER_CONSECUTIVE_REORG_PARTITIONS) < 0) goto error;
#endif
#ifdef ER_REORG_OUTSIDE_RANGE
    if (PyModule_AddIntMacro(constants, ER_REORG_OUTSIDE_RANGE) < 0) goto error;
#endif
#ifdef ER_PARTITION_FUNCTION_FAILURE
    if (PyModule_AddIntMacro(constants, ER_PARTITION_FUNCTION_FAILURE) < 0) goto error;
#endif
#ifdef ER_PART_STATE_ERROR
    if (PyModule_AddIntMacro(constants, ER_PART_STATE_ERROR) < 0) goto error;
#endif
#ifdef ER_LIMITED_PART_RANGE
    if (PyModule_AddIntMacro(constants, ER_LIMITED_PART_RANGE) < 0) goto error;
#endif
#ifdef ER_PLUGIN_IS_NOT_LOADED
    if (PyModule_AddIntMacro(constants, ER_PLUGIN_IS_NOT_LOADED) < 0) goto error;
#endif
#ifdef ER_WRONG_VALUE
    if (PyModule_AddIntMacro(constants, ER_WRONG_VALUE) < 0) goto error;
#endif
#ifdef ER_NO_PARTITION_FOR_GIVEN_VALUE
    if (PyModule_AddIntMacro(constants, ER_NO_PARTITION_FOR_GIVEN_VALUE) < 0) goto error;
#endif
#ifdef ER_FILEGROUP_OPTION_ONLY_ONCE
    if (PyModule_AddIntMacro(constants, ER_FILEGROUP_OPTION_ONLY_ONCE) < 0) goto error;
#endif
#ifdef ER_CREATE_FILEGROUP_FAILED
    if (PyModule_AddIntMacro(constants, ER_CREATE_FILEGROUP_FAILED) < 0) goto error;
#endif
#ifdef ER_DROP_FILEGROUP_FAILED
    if (PyModule_AddIntMacro(constants, ER_DROP_FILEGROUP_FAILED) < 0) goto error;
#endif
#ifdef ER_TABLESPACE_AUTO_EXTEND_ERROR
    if (PyModule_AddIntMacro(constants, ER_TABLESPACE_AUTO_EXTEND_ERROR) < 0) goto error;
#endif
#ifdef ER_WRONG_SIZE_NUMBER
    if (PyModule_AddIntMacro(constants, ER_WRONG_SIZE_NUMBER) < 0) goto error;
#endif
#ifdef ER_SIZE_OVERFLOW_ERROR
    if (PyModule_AddIntMacro(constants, ER_SIZE_OVERFLOW_ERROR) < 0) goto error;
#endif
#ifdef ER_ALTER_FILEGROUP_FAILED
    if (PyModule_AddIntMacro(constants, ER_ALTER_FILEGROUP_FAILED) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_LOGGING_FAILED
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_LOGGING_FAILED) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_WRONG_TABLE_DEF
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_WRONG_TABLE_DEF) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_RBR_TO_SBR
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_RBR_TO_SBR) < 0) goto error;
#endif
#ifdef ER_EVENT_ALREADY_EXISTS
    if (PyModule_AddIntMacro(constants, ER_EVENT_ALREADY_EXISTS) < 0) goto error;
#endif
#ifdef ER_EVENT_STORE_FAILED
    if (PyModule_AddIntMacro(constants, ER_EVENT_STORE_FAILED) < 0) goto error;
#endif
#ifdef ER_EVENT_DOES_NOT_EXIST
    if (PyModule_AddIntMacro(constants, ER_EVENT_DOES_NOT_EXIST) < 0) goto error;
#endif
#ifdef ER_EVENT_CANT_ALTER
    if (PyModule_AddIntMacro(constants, ER_EVENT_CANT_ALTER) < 0) goto error;
#endif
#ifdef ER_EVENT_DROP_FAILED
    if (PyModule_AddIntMacro(constants, ER_EVENT_DROP_FAILED) < 0) goto error;
#endif
#ifdef ER_EVENT_INTERVAL_NOT_POSITIVE_OR_TOO_BIG
    if (PyModule_AddIntMacro(constants, ER_EVENT_INTERVAL_NOT_POSITIVE_OR_TOO_BIG) < 0) goto error;
#endif
#ifdef ER_EVENT_ENDS_BEFORE_STARTS
    if (PyModule_AddIntMacro(constants, ER_EVENT_ENDS_BEFORE_STARTS) < 0) goto error;
#endif
#ifdef ER_EVENT_EXEC_TIME_IN_THE_PAST
    if (PyModule_AddIntMacro(constants, ER_EVENT_EXEC_TIME_IN_THE_PAST) < 0) goto error;
#endif
#ifdef ER_EVENT_OPEN_TABLE_FAILED
    if (PyModule_AddIntMacro(constants, ER_EVENT_OPEN_TABLE_FAILED) < 0) goto error;
#endif
#ifdef ER_EVENT_NEITHER_M_EXPR_NOR_M_AT
    if (PyModule_AddIntMacro(constants, ER_EVENT_NEITHER_M_EXPR_NOR_M_AT) < 0) goto error;
#endif
#ifdef ER_OBSOLETE_COL_COUNT_DOESNT_MATCH_CORRUPTED
    if (PyModule_AddIntMacro(constants, ER_OBSOLETE_COL_COUNT_DOESNT_MATCH_CORRUPTED) < 0) goto error;
#endif
#ifdef ER_OBSOLETE_CANNOT_LOAD_FROM_TABLE
    if (PyModule_AddIntMacro(constants, ER_OBSOLETE_CANNOT_LOAD_FROM_TABLE) < 0) goto error;
#endif
#ifdef ER_EVENT_CANNOT_DELETE
    if (PyModule_AddIntMacro(constants, ER_EVENT_CANNOT_DELETE) < 0) goto error;
#endif
#ifdef ER_EVENT_COMPILE_ERROR
    if (PyModule_AddIntMacro(constants, ER_EVENT_COMPILE_ERROR) < 0) goto error;
#endif
#ifdef ER_EVENT_SAME_NAME
    if (PyModule_AddIntMacro(constants, ER_EVENT_SAME_NAME) < 0) goto error;
#endif
#ifdef ER_EVENT_DATA_TOO_LONG
    if (PyModule_AddIntMacro(constants, ER_EVENT_DATA_TOO_LONG) < 0) goto error;
#endif
#ifdef ER_DROP_INDEX_FK
    if (PyModule_AddIntMacro(constants, ER_DROP_INDEX_FK) < 0) goto error;
#endif
#ifdef ER_WARN_DEPRECATED_SYNTAX_WITH_VER
    if (PyModule_AddIntMacro(constants, ER_WARN_DEPRECATED_SYNTAX_WITH_VER) < 0) goto error;
#endif
#ifdef ER_CANT_WRITE_LOCK_LOG_TABLE
    if (PyModule_AddIntMacro(constants, ER_CANT_WRITE_LOCK_LOG_TABLE) < 0) goto error;
#endif
#ifdef ER_CANT_LOCK_LOG_TABLE
    if (PyModule_AddIntMacro(constants, ER_CANT_LOCK_LOG_TABLE) < 0) goto error;
#endif
#ifdef ER_FOREIGN_DUPLICATE_KEY_OLD_UNUSED
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_DUPLICATE_KEY_OLD_UNUSED) < 0) goto error;
#endif
#ifdef ER_COL_COUNT_DOESNT_MATCH_PLEASE_UPDATE
    if (PyModule_AddIntMacro(constants, ER_COL_COUNT_DOESNT_MATCH_PLEASE_UPDATE) < 0) goto error;
#endif
#ifdef ER_TEMP_TABLE_PREVENTS_SWITCH_OUT_OF_RBR
    if (PyModule_AddIntMacro(constants, ER_TEMP_TABLE_PREVENTS_SWITCH_OUT_OF_RBR) < 0) goto error;
#endif
#ifdef ER_STORED_FUNCTION_PREVENTS_SWITCH_BINLOG_FORMAT
    if (PyModule_AddIntMacro(constants, ER_STORED_FUNCTION_PREVENTS_SWITCH_BINLOG_FORMAT) < 0) goto error;
#endif
#ifdef ER_NDB_CANT_SWITCH_BINLOG_FORMAT
    if (PyModule_AddIntMacro(constants, ER_NDB_CANT_SWITCH_BINLOG_FORMAT) < 0) goto error;
#endif
#ifdef ER_PARTITION_NO_TEMPORARY
    if (PyModule_AddIntMacro(constants, ER_PARTITION_NO_TEMPORARY) < 0) goto error;
#endif
#ifdef ER_PARTITION_CONST_DOMAIN_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_CONST_DOMAIN_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_FUNCTION_IS_NOT_ALLOWED
    if (PyModule_AddIntMacro(constants, ER_PARTITION_FUNCTION_IS_NOT_ALLOWED) < 0) goto error;
#endif
#ifdef ER_DDL_LOG_ERROR
    if (PyModule_AddIntMacro(constants, ER_DDL_LOG_ERROR) < 0) goto error;
#endif
#ifdef ER_NULL_IN_VALUES_LESS_THAN
    if (PyModule_AddIntMacro(constants, ER_NULL_IN_VALUES_LESS_THAN) < 0) goto error;
#endif
#ifdef ER_WRONG_PARTITION_NAME
    if (PyModule_AddIntMacro(constants, ER_WRONG_PARTITION_NAME) < 0) goto error;
#endif
#ifdef ER_CANT_CHANGE_TX_CHARACTERISTICS
    if (PyModule_AddIntMacro(constants, ER_CANT_CHANGE_TX_CHARACTERISTICS) < 0) goto error;
#endif
#ifdef ER_DUP_ENTRY_AUTOINCREMENT_CASE
    if (PyModule_AddIntMacro(constants, ER_DUP_ENTRY_AUTOINCREMENT_CASE) < 0) goto error;
#endif
#ifdef ER_EVENT_MODIFY_QUEUE_ERROR
    if (PyModule_AddIntMacro(constants, ER_EVENT_MODIFY_QUEUE_ERROR) < 0) goto error;
#endif
#ifdef ER_EVENT_SET_VAR_ERROR
    if (PyModule_AddIntMacro(constants, ER_EVENT_SET_VAR_ERROR) < 0) goto error;
#endif
#ifdef ER_PARTITION_MERGE_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_MERGE_ERROR) < 0) goto error;
#endif
#ifdef ER_CANT_ACTIVATE_LOG
    if (PyModule_AddIntMacro(constants, ER_CANT_ACTIVATE_LOG) < 0) goto error;
#endif
#ifdef ER_RBR_NOT_AVAILABLE
    if (PyModule_AddIntMacro(constants, ER_RBR_NOT_AVAILABLE) < 0) goto error;
#endif
#ifdef ER_BASE64_DECODE_ERROR
    if (PyModule_AddIntMacro(constants, ER_BASE64_DECODE_ERROR) < 0) goto error;
#endif
#ifdef ER_EVENT_RECURSION_FORBIDDEN
    if (PyModule_AddIntMacro(constants, ER_EVENT_RECURSION_FORBIDDEN) < 0) goto error;
#endif
#ifdef ER_EVENTS_DB_ERROR
    if (PyModule_AddIntMacro(constants, ER_EVENTS_DB_ERROR) < 0) goto error;
#endif
#ifdef ER_ONLY_INTEGERS_ALLOWED
    if (PyModule_AddIntMacro(constants, ER_ONLY_INTEGERS_ALLOWED) < 0) goto error;
#endif
#ifdef ER_UNSUPORTED_LOG_ENGINE
    if (PyModule_AddIntMacro(constants, ER_UNSUPORTED_LOG_ENGINE) < 0) goto error;
#endif
#ifdef ER_BAD_LOG_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_BAD_LOG_STATEMENT) < 0) goto error;
#endif
#ifdef ER_CANT_RENAME_LOG_TABLE
    if (PyModule_AddIntMacro(constants, ER_CANT_RENAME_LOG_TABLE) < 0) goto error;
#endif
#ifdef ER_WRONG_PARAMCOUNT_TO_NATIVE_FCT
    if (PyModule_AddIntMacro(constants, ER_WRONG_PARAMCOUNT_TO_NATIVE_FCT) < 0) goto error;
#endif
#ifdef ER_WRONG_PARAMETERS_TO_NATIVE_FCT
    if (PyModule_AddIntMacro(constants, ER_WRONG_PARAMETERS_TO_NATIVE_FCT) < 0) goto error;
#endif
#ifdef ER_WRONG_PARAMETERS_TO_STORED_FCT
    if (PyModule_AddIntMacro(constants, ER_WRONG_PARAMETERS_TO_STORED_FCT) < 0) goto error;
#endif
#ifdef ER_NATIVE_FCT_NAME_COLLISION
    if (PyModule_AddIntMacro(constants, ER_NATIVE_FCT_NAME_COLLISION) < 0) goto error;
#endif
#ifdef ER_DUP_ENTRY_WITH_KEY_NAME
    if (PyModule_AddIntMacro(constants, ER_DUP_ENTRY_WITH_KEY_NAME) < 0) goto error;
#endif
#ifdef ER_BINLOG_PURGE_EMFILE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_PURGE_EMFILE) < 0) goto error;
#endif
#ifdef ER_EVENT_CANNOT_CREATE_IN_THE_PAST
    if (PyModule_AddIntMacro(constants, ER_EVENT_CANNOT_CREATE_IN_THE_PAST) < 0) goto error;
#endif
#ifdef ER_EVENT_CANNOT_ALTER_IN_THE_PAST
    if (PyModule_AddIntMacro(constants, ER_EVENT_CANNOT_ALTER_IN_THE_PAST) < 0) goto error;
#endif
#ifdef ER_SLAVE_INCIDENT
    if (PyModule_AddIntMacro(constants, ER_SLAVE_INCIDENT) < 0) goto error;
#endif
#ifdef ER_NO_PARTITION_FOR_GIVEN_VALUE_SILENT
    if (PyModule_AddIntMacro(constants, ER_NO_PARTITION_FOR_GIVEN_VALUE_SILENT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_STATEMENT) < 0) goto error;
#endif
#ifdef ER_SLAVE_FATAL_ERROR
    if (PyModule_AddIntMacro(constants, ER_SLAVE_FATAL_ERROR) < 0) goto error;
#endif
#ifdef ER_SLAVE_RELAY_LOG_READ_FAILURE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_RELAY_LOG_READ_FAILURE) < 0) goto error;
#endif
#ifdef ER_SLAVE_RELAY_LOG_WRITE_FAILURE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_RELAY_LOG_WRITE_FAILURE) < 0) goto error;
#endif
#ifdef ER_SLAVE_CREATE_EVENT_FAILURE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_CREATE_EVENT_FAILURE) < 0) goto error;
#endif
#ifdef ER_SLAVE_MASTER_COM_FAILURE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_MASTER_COM_FAILURE) < 0) goto error;
#endif
#ifdef ER_BINLOG_LOGGING_IMPOSSIBLE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_LOGGING_IMPOSSIBLE) < 0) goto error;
#endif
#ifdef ER_VIEW_NO_CREATION_CTX
    if (PyModule_AddIntMacro(constants, ER_VIEW_NO_CREATION_CTX) < 0) goto error;
#endif
#ifdef ER_VIEW_INVALID_CREATION_CTX
    if (PyModule_AddIntMacro(constants, ER_VIEW_INVALID_CREATION_CTX) < 0) goto error;
#endif
#ifdef ER_SR_INVALID_CREATION_CTX
    if (PyModule_AddIntMacro(constants, ER_SR_INVALID_CREATION_CTX) < 0) goto error;
#endif
#ifdef ER_TRG_CORRUPTED_FILE
    if (PyModule_AddIntMacro(constants, ER_TRG_CORRUPTED_FILE) < 0) goto error;
#endif
#ifdef ER_TRG_NO_CREATION_CTX
    if (PyModule_AddIntMacro(constants, ER_TRG_NO_CREATION_CTX) < 0) goto error;
#endif
#ifdef ER_TRG_INVALID_CREATION_CTX
    if (PyModule_AddIntMacro(constants, ER_TRG_INVALID_CREATION_CTX) < 0) goto error;
#endif
#ifdef ER_EVENT_INVALID_CREATION_CTX
    if (PyModule_AddIntMacro(constants, ER_EVENT_INVALID_CREATION_CTX) < 0) goto error;
#endif
#ifdef ER_TRG_CANT_OPEN_TABLE
    if (PyModule_AddIntMacro(constants, ER_TRG_CANT_OPEN_TABLE) < 0) goto error;
#endif
#ifdef ER_CANT_CREATE_SROUTINE
    if (PyModule_AddIntMacro(constants, ER_CANT_CREATE_SROUTINE) < 0) goto error;
#endif
#ifdef ER_NEVER_USED
    if (PyModule_AddIntMacro(constants, ER_NEVER_USED) < 0) goto error;
#endif
#ifdef ER_NO_FORMAT_DESCRIPTION_EVENT_BEFORE_BINLOG_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_NO_FORMAT_DESCRIPTION_EVENT_BEFORE_BINLOG_STATEMENT) < 0) goto error;
#endif
#ifdef ER_SLAVE_CORRUPT_EVENT
    if (PyModule_AddIntMacro(constants, ER_SLAVE_CORRUPT_EVENT) < 0) goto error;
#endif
#ifdef ER_LOAD_DATA_INVALID_COLUMN
    if (PyModule_AddIntMacro(constants, ER_LOAD_DATA_INVALID_COLUMN) < 0) goto error;
#endif
#ifdef ER_LOG_PURGE_NO_FILE
    if (PyModule_AddIntMacro(constants, ER_LOG_PURGE_NO_FILE) < 0) goto error;
#endif
#ifdef ER_XA_RBTIMEOUT
    if (PyModule_AddIntMacro(constants, ER_XA_RBTIMEOUT) < 0) goto error;
#endif
#ifdef ER_XA_RBDEADLOCK
    if (PyModule_AddIntMacro(constants, ER_XA_RBDEADLOCK) < 0) goto error;
#endif
#ifdef ER_NEED_REPREPARE
    if (PyModule_AddIntMacro(constants, ER_NEED_REPREPARE) < 0) goto error;
#endif
#ifdef ER_DELAYED_NOT_SUPPORTED
    if (PyModule_AddIntMacro(constants, ER_DELAYED_NOT_SUPPORTED) < 0) goto error;
#endif
#ifdef WARN_NO_MASTER_INFO
    if (PyModule_AddIntMacro(constants, WARN_NO_MASTER_INFO) < 0) goto error;
#endif
#ifdef WARN_OPTION_IGNORED
    if (PyModule_AddIntMacro(constants, WARN_OPTION_IGNORED) < 0) goto error;
#endif
#ifdef WARN_PLUGIN_DELETE_BUILTIN
    if (PyModule_AddIntMacro(constants, WARN_PLUGIN_DELETE_BUILTIN) < 0) goto error;
#endif
#ifdef WARN_PLUGIN_BUSY
    if (PyModule_AddIntMacro(constants, WARN_PLUGIN_BUSY) < 0) goto error;
#endif
#ifdef ER_VARIABLE_IS_READONLY
    if (PyModule_AddIntMacro(constants, ER_VARIABLE_IS_READONLY) < 0) goto error;
#endif
#ifdef ER_WARN_ENGINE_TRANSACTION_ROLLBACK
    if (PyModule_AddIntMacro(constants, ER_WARN_ENGINE_TRANSACTION_ROLLBACK) < 0) goto error;
#endif
#ifdef ER_SLAVE_HEARTBEAT_FAILURE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_HEARTBEAT_FAILURE) < 0) goto error;
#endif
#ifdef ER_SLAVE_HEARTBEAT_VALUE_OUT_OF_RANGE
    if (PyModule_AddIntMacro(constants, ER_SLAVE_HEARTBEAT_VALUE_OUT_OF_RANGE) < 0) goto error;
#endif
#ifdef ER_NDB_REPLICATION_SCHEMA_ERROR
    if (PyModule_AddIntMacro(constants, ER_NDB_REPLICATION_SCHEMA_ERROR) < 0) goto error;
#endif
#ifdef ER_CONFLICT_FN_PARSE_ERROR
    if (PyModule_AddIntMacro(constants, ER_CONFLICT_FN_PARSE_ERROR) < 0) goto error;
#endif
#ifdef ER_EXCEPTIONS_WRITE_ERROR
    if (PyModule_AddIntMacro(constants, ER_EXCEPTIONS_WRITE_ERROR) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_TABLE_COMMENT
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_TABLE_COMMENT) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_FIELD_COMMENT
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_FIELD_COMMENT) < 0) goto error;
#endif
#ifdef ER_FUNC_INEXISTENT_NAME_COLLISION
    if (PyModule_AddIntMacro(constants, ER_FUNC_INEXISTENT_NAME_COLLISION) < 0) goto error;
#endif
#ifdef ER_DATABASE_NAME
    if (PyModule_AddIntMacro(constants, ER_DATABASE_NAME) < 0) goto error;
#endif
#ifdef ER_TABLE_NAME
    if (PyModule_AddIntMacro(constants, ER_TABLE_NAME) < 0) goto error;
#endif
#ifdef ER_PARTITION_NAME
    if (PyModule_AddIntMacro(constants, ER_PARTITION_NAME) < 0) goto error;
#endif
#ifdef ER_SUBPARTITION_NAME
    if (PyModule_AddIntMacro(constants, ER_SUBPARTITION_NAME) < 0) goto error;
#endif
#ifdef ER_TEMPORARY_NAME
    if (PyModule_AddIntMacro(constants, ER_TEMPORARY_NAME) < 0) goto error;
#endif
#ifdef ER_RENAMED_NAME
    if (PyModule_AddIntMacro(constants, ER_RENAMED_NAME) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_CONCURRENT_TRXS
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_CONCURRENT_TRXS) < 0) goto error;
#endif
#ifdef WARN_NON_ASCII_SEPARATOR_NOT_IMPLEMENTED
    if (PyModule_AddIntMacro(constants, WARN_NON_ASCII_SEPARATOR_NOT_IMPLEMENTED) < 0) goto error;
#endif
#ifdef ER_DEBUG_SYNC_TIMEOUT
    if (PyModule_AddIntMacro(constants, ER_DEBUG_SYNC_TIMEOUT) < 0) goto error;
#endif
#ifdef ER_DEBUG_SYNC_HIT_LIMIT
    if (PyModule_AddIntMacro(constants, ER_DEBUG_SYNC_HIT_LIMIT) < 0) goto error;
#endif
#ifdef ER_DUP_SIGNAL_SET
    if (PyModule_AddIntMacro(constants, ER_DUP_SIGNAL_SET) < 0) goto error;
#endif
#ifdef ER_SIGNAL_WARN
    if (PyModule_AddIntMacro(constants, ER_SIGNAL_WARN) < 0) goto error;
#endif
#ifdef ER_SIGNAL_NOT_FOUND
    if (PyModule_AddIntMacro(constants, ER_SIGNAL_NOT_FOUND) < 0) goto error;
#endif
#ifdef ER_SIGNAL_EXCEPTION
    if (PyModule_AddIntMacro(constants, ER_SIGNAL_EXCEPTION) < 0) goto error;
#endif
#ifdef ER_RESIGNAL_WITHOUT_ACTIVE_HANDLER
    if (PyModule_AddIntMacro(constants, ER_RESIGNAL_WITHOUT_ACTIVE_HANDLER) < 0) goto error;
#endif
#ifdef ER_SIGNAL_BAD_CONDITION_TYPE
    if (PyModule_AddIntMacro(constants, ER_SIGNAL_BAD_CONDITION_TYPE) < 0) goto error;
#endif
#ifdef WARN_COND_ITEM_TRUNCATED
    if (PyModule_AddIntMacro(constants, WARN_COND_ITEM_TRUNCATED) < 0) goto error;
#endif
#ifdef ER_COND_ITEM_TOO_LONG
    if (PyModule_AddIntMacro(constants, ER_COND_ITEM_TOO_LONG) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_LOCALE
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_LOCALE) < 0) goto error;
#endif
#ifdef ER_SLAVE_IGNORE_SERVER_IDS
    if (PyModule_AddIntMacro(constants, ER_SLAVE_IGNORE_SERVER_IDS) < 0) goto error;
#endif
#ifdef ER_QUERY_CACHE_DISABLED
    if (PyModule_AddIntMacro(constants, ER_QUERY_CACHE_DISABLED) < 0) goto error;
#endif
#ifdef ER_SAME_NAME_PARTITION_FIELD
    if (PyModule_AddIntMacro(constants, ER_SAME_NAME_PARTITION_FIELD) < 0) goto error;
#endif
#ifdef ER_PARTITION_COLUMN_LIST_ERROR
    if (PyModule_AddIntMacro(constants, ER_PARTITION_COLUMN_LIST_ERROR) < 0) goto error;
#endif
#ifdef ER_WRONG_TYPE_COLUMN_VALUE_ERROR
    if (PyModule_AddIntMacro(constants, ER_WRONG_TYPE_COLUMN_VALUE_ERROR) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_PARTITION_FUNC_FIELDS_ERROR
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_PARTITION_FUNC_FIELDS_ERROR) < 0) goto error;
#endif
#ifdef ER_MAXVALUE_IN_VALUES_IN
    if (PyModule_AddIntMacro(constants, ER_MAXVALUE_IN_VALUES_IN) < 0) goto error;
#endif
#ifdef ER_TOO_MANY_VALUES_ERROR
    if (PyModule_AddIntMacro(constants, ER_TOO_MANY_VALUES_ERROR) < 0) goto error;
#endif
#ifdef ER_ROW_SINGLE_PARTITION_FIELD_ERROR
    if (PyModule_AddIntMacro(constants, ER_ROW_SINGLE_PARTITION_FIELD_ERROR) < 0) goto error;
#endif
#ifdef ER_FIELD_TYPE_NOT_ALLOWED_AS_PARTITION_FIELD
    if (PyModule_AddIntMacro(constants, ER_FIELD_TYPE_NOT_ALLOWED_AS_PARTITION_FIELD) < 0) goto error;
#endif
#ifdef ER_PARTITION_FIELDS_TOO_LONG
    if (PyModule_AddIntMacro(constants, ER_PARTITION_FIELDS_TOO_LONG) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_ENGINE_AND_STMT_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_ENGINE_AND_STMT_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_MODE_AND_STMT_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_MODE_AND_STMT_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_AND_STMT_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_AND_STMT_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_INJECTION_AND_STMT_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_INJECTION_AND_STMT_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_STMT_MODE_AND_ROW_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_STMT_MODE_AND_ROW_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_ROW_INJECTION_AND_STMT_MODE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_ROW_INJECTION_AND_STMT_MODE) < 0) goto error;
#endif
#ifdef ER_BINLOG_MULTIPLE_ENGINES_AND_SELF_LOGGING_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_MULTIPLE_ENGINES_AND_SELF_LOGGING_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_LIMIT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_LIMIT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_INSERT_DELAYED
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_INSERT_DELAYED) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_SYSTEM_TABLE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_SYSTEM_TABLE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_AUTOINC_COLUMNS
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_AUTOINC_COLUMNS) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_UDF
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_UDF) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_SYSTEM_VARIABLE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_SYSTEM_VARIABLE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_SYSTEM_FUNCTION
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_SYSTEM_FUNCTION) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_NONTRANS_AFTER_TRANS
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_NONTRANS_AFTER_TRANS) < 0) goto error;
#endif
#ifdef ER_MESSAGE_AND_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_MESSAGE_AND_STATEMENT) < 0) goto error;
#endif
#ifdef ER_SLAVE_CONVERSION_FAILED
    if (PyModule_AddIntMacro(constants, ER_SLAVE_CONVERSION_FAILED) < 0) goto error;
#endif
#ifdef ER_SLAVE_CANT_CREATE_CONVERSION
    if (PyModule_AddIntMacro(constants, ER_SLAVE_CANT_CREATE_CONVERSION) < 0) goto error;
#endif
#ifdef ER_INSIDE_TRANSACTION_PREVENTS_SWITCH_BINLOG_FORMAT
    if (PyModule_AddIntMacro(constants, ER_INSIDE_TRANSACTION_PREVENTS_SWITCH_BINLOG_FORMAT) < 0) goto error;
#endif
#ifdef ER_PATH_LENGTH
    if (PyModule_AddIntMacro(constants, ER_PATH_LENGTH) < 0) goto error;
#endif
#ifdef ER_WARN_DEPRECATED_SYNTAX_NO_REPLACEMENT
    if (PyModule_AddIntMacro(constants, ER_WARN_DEPRECATED_SYNTAX_NO_REPLACEMENT) < 0) goto error;
#endif
#ifdef ER_WRONG_NATIVE_TABLE_STRUCTURE
    if (PyModule_AddIntMacro(constants, ER_WRONG_NATIVE_TABLE_STRUCTURE) < 0) goto error;
#endif
#ifdef ER_WRONG_PERFSCHEMA_USAGE
    if (PyModule_AddIntMacro(constants, ER_WRONG_PERFSCHEMA_USAGE) < 0) goto error;
#endif
#ifdef ER_WARN_I_S_SKIPPED_TABLE
    if (PyModule_AddIntMacro(constants, ER_WARN_I_S_SKIPPED_TABLE) < 0) goto error;
#endif
#ifdef ER_INSIDE_TRANSACTION_PREVENTS_SWITCH_BINLOG_DIRECT
    if (PyModule_AddIntMacro(constants, ER_INSIDE_TRANSACTION_PREVENTS_SWITCH_BINLOG_DIRECT) < 0) goto error;
#endif
#ifdef ER_STORED_FUNCTION_PREVENTS_SWITCH_BINLOG_DIRECT
    if (PyModule_AddIntMacro(constants, ER_STORED_FUNCTION_PREVENTS_SWITCH_BINLOG_DIRECT) < 0) goto error;
#endif
#ifdef ER_SPATIAL_MUST_HAVE_GEOM_COL
    if (PyModule_AddIntMacro(constants, ER_SPATIAL_MUST_HAVE_GEOM_COL) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_INDEX_COMMENT
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_INDEX_COMMENT) < 0) goto error;
#endif
#ifdef ER_LOCK_ABORTED
    if (PyModule_AddIntMacro(constants, ER_LOCK_ABORTED) < 0) goto error;
#endif
#ifdef ER_DATA_OUT_OF_RANGE
    if (PyModule_AddIntMacro(constants, ER_DATA_OUT_OF_RANGE) < 0) goto error;
#endif
#ifdef ER_WRONG_SPVAR_TYPE_IN_LIMIT
    if (PyModule_AddIntMacro(constants, ER_WRONG_SPVAR_TYPE_IN_LIMIT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_MULTIPLE_ENGINES_AND_SELF_LOGGING_ENGINE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_MULTIPLE_ENGINES_AND_SELF_LOGGING_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_MIXED_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_MIXED_STATEMENT) < 0) goto error;
#endif
#ifdef ER_INSIDE_TRANSACTION_PREVENTS_SWITCH_SQL_LOG_BIN
    if (PyModule_AddIntMacro(constants, ER_INSIDE_TRANSACTION_PREVENTS_SWITCH_SQL_LOG_BIN) < 0) goto error;
#endif
#ifdef ER_STORED_FUNCTION_PREVENTS_SWITCH_SQL_LOG_BIN
    if (PyModule_AddIntMacro(constants, ER_STORED_FUNCTION_PREVENTS_SWITCH_SQL_LOG_BIN) < 0) goto error;
#endif
#ifdef ER_FAILED_READ_FROM_PAR_FILE
    if (PyModule_AddIntMacro(constants, ER_FAILED_READ_FROM_PAR_FILE) < 0) goto error;
#endif
#ifdef ER_VALUES_IS_NOT_INT_TYPE_ERROR
    if (PyModule_AddIntMacro(constants, ER_VALUES_IS_NOT_INT_TYPE_ERROR) < 0) goto error;
#endif
#ifdef ER_ACCESS_DENIED_NO_PASSWORD_ERROR
    if (PyModule_AddIntMacro(constants, ER_ACCESS_DENIED_NO_PASSWORD_ERROR) < 0) goto error;
#endif
#ifdef ER_SET_PASSWORD_AUTH_PLUGIN
    if (PyModule_AddIntMacro(constants, ER_SET_PASSWORD_AUTH_PLUGIN) < 0) goto error;
#endif
#ifdef ER_GRANT_PLUGIN_USER_EXISTS
    if (PyModule_AddIntMacro(constants, ER_GRANT_PLUGIN_USER_EXISTS) < 0) goto error;
#endif
#ifdef ER_TRUNCATE_ILLEGAL_FK
    if (PyModule_AddIntMacro(constants, ER_TRUNCATE_ILLEGAL_FK) < 0) goto error;
#endif
#ifdef ER_PLUGIN_IS_PERMANENT
    if (PyModule_AddIntMacro(constants, ER_PLUGIN_IS_PERMANENT) < 0) goto error;
#endif
#ifdef ER_SLAVE_HEARTBEAT_VALUE_OUT_OF_RANGE_MIN
    if (PyModule_AddIntMacro(constants, ER_SLAVE_HEARTBEAT_VALUE_OUT_OF_RANGE_MIN) < 0) goto error;
#endif
#ifdef ER_SLAVE_HEARTBEAT_VALUE_OUT_OF_RANGE_MAX
    if (PyModule_AddIntMacro(constants, ER_SLAVE_HEARTBEAT_VALUE_OUT_OF_RANGE_MAX) < 0) goto error;
#endif
#ifdef ER_STMT_CACHE_FULL
    if (PyModule_AddIntMacro(constants, ER_STMT_CACHE_FULL) < 0) goto error;
#endif
#ifdef ER_MULTI_UPDATE_KEY_CONFLICT
    if (PyModule_AddIntMacro(constants, ER_MULTI_UPDATE_KEY_CONFLICT) < 0) goto error;
#endif
#ifdef ER_TABLE_NEEDS_REBUILD
    if (PyModule_AddIntMacro(constants, ER_TABLE_NEEDS_REBUILD) < 0) goto error;
#endif
#ifdef WARN_OPTION_BELOW_LIMIT
    if (PyModule_AddIntMacro(constants, WARN_OPTION_BELOW_LIMIT) < 0) goto error;
#endif
#ifdef ER_INDEX_COLUMN_TOO_LONG
    if (PyModule_AddIntMacro(constants, ER_INDEX_COLUMN_TOO_LONG) < 0) goto error;
#endif
#ifdef ER_ERROR_IN_TRIGGER_BODY
    if (PyModule_AddIntMacro(constants, ER_ERROR_IN_TRIGGER_BODY) < 0) goto error;
#endif
#ifdef ER_ERROR_IN_UNKNOWN_TRIGGER_BODY
    if (PyModule_AddIntMacro(constants, ER_ERROR_IN_UNKNOWN_TRIGGER_BODY) < 0) goto error;
#endif
#ifdef ER_INDEX_CORRUPT
    if (PyModule_AddIntMacro(constants, ER_INDEX_CORRUPT) < 0) goto error;
#endif
#ifdef ER_UNDO_RECORD_TOO_BIG
    if (PyModule_AddIntMacro(constants, ER_UNDO_RECORD_TOO_BIG) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_INSERT_IGNORE_SELECT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_INSERT_IGNORE_SELECT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_INSERT_SELECT_UPDATE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_INSERT_SELECT_UPDATE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_REPLACE_SELECT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_REPLACE_SELECT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_CREATE_IGNORE_SELECT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_CREATE_IGNORE_SELECT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_CREATE_REPLACE_SELECT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_CREATE_REPLACE_SELECT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_UPDATE_IGNORE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_UPDATE_IGNORE) < 0) goto error;
#endif
#ifdef ER_PLUGIN_NO_UNINSTALL
    if (PyModule_AddIntMacro(constants, ER_PLUGIN_NO_UNINSTALL) < 0) goto error;
#endif
#ifdef ER_PLUGIN_NO_INSTALL
    if (PyModule_AddIntMacro(constants, ER_PLUGIN_NO_INSTALL) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_WRITE_AUTOINC_SELECT
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_WRITE_AUTOINC_SELECT) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_CREATE_SELECT_AUTOINC
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_CREATE_SELECT_AUTOINC) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_INSERT_TWO_KEYS
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_INSERT_TWO_KEYS) < 0) goto error;
#endif
#ifdef ER_TABLE_IN_FK_CHECK
    if (PyModule_AddIntMacro(constants, ER_TABLE_IN_FK_CHECK) < 0) goto error;
#endif
#ifdef ER_UNSUPPORTED_ENGINE
    if (PyModule_AddIntMacro(constants, ER_UNSUPPORTED_ENGINE) < 0) goto error;
#endif
#ifdef ER_BINLOG_UNSAFE_AUTOINC_NOT_FIRST
    if (PyModule_AddIntMacro(constants, ER_BINLOG_UNSAFE_AUTOINC_NOT_FIRST) < 0) goto error;
#endif
#ifdef ER_CANNOT_LOAD_FROM_TABLE_V2
    if (PyModule_AddIntMacro(constants, ER_CANNOT_LOAD_FROM_TABLE_V2) < 0) goto error;
#endif
#ifdef ER_MASTER_DELAY_VALUE_OUT_OF_RANGE
    if (PyModule_AddIntMacro(constants, ER_MASTER_DELAY_VALUE_OUT_OF_RANGE) < 0) goto error;
#endif
#ifdef ER_ONLY_FD_AND_RBR_EVENTS_ALLOWED_IN_BINLOG_STATEMENT
    if (PyModule_AddIntMacro(constants, ER_ONLY_FD_AND_RBR_EVENTS_ALLOWED_IN_BINLOG_STATEMENT) < 0) goto error;
#endif
#ifdef ER_PARTITION_EXCHANGE_DIFFERENT_OPTION
    if (PyModule_AddIntMacro(constants, ER_PARTITION_EXCHANGE_DIFFERENT_OPTION) < 0) goto error;
#endif
#ifdef ER_PARTITION_EXCHANGE_PART_TABLE
    if (PyModule_AddIntMacro(constants, ER_PARTITION_EXCHANGE_PART_TABLE) < 0) goto error;
#endif
#ifdef ER_PARTITION_EXCHANGE_TEMP_TABLE
    if (PyModule_AddIntMacro(constants, ER_PARTITION_EXCHANGE_TEMP_TABLE) < 0) goto error;
#endif
#ifdef ER_PARTITION_INSTEAD_OF_SUBPARTITION
    if (PyModule_AddIntMacro(constants, ER_PARTITION_INSTEAD_OF_SUBPARTITION) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_PARTITION
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_PARTITION) < 0) goto error;
#endif
#ifdef ER_TABLES_DIFFERENT_METADATA
    if (PyModule_AddIntMacro(constants, ER_TABLES_DIFFERENT_METADATA) < 0) goto error;
#endif
#ifdef ER_ROW_DOES_NOT_MATCH_PARTITION
    if (PyModule_AddIntMacro(constants, ER_ROW_DOES_NOT_MATCH_PARTITION) < 0) goto error;
#endif
#ifdef ER_BINLOG_CACHE_SIZE_GREATER_THAN_MAX
    if (PyModule_AddIntMacro(constants, ER_BINLOG_CACHE_SIZE_GREATER_THAN_MAX) < 0) goto error;
#endif
#ifdef ER_WARN_INDEX_NOT_APPLICABLE
    if (PyModule_AddIntMacro(constants, ER_WARN_INDEX_NOT_APPLICABLE) < 0) goto error;
#endif
#ifdef ER_PARTITION_EXCHANGE_FOREIGN_KEY
    if (PyModule_AddIntMacro(constants, ER_PARTITION_EXCHANGE_FOREIGN_KEY) < 0) goto error;
#endif
#ifdef ER_NO_SUCH_KEY_VALUE
    if (PyModule_AddIntMacro(constants, ER_NO_SUCH_KEY_VALUE) < 0) goto error;
#endif
#ifdef ER_RPL_INFO_DATA_TOO_LONG
    if (PyModule_AddIntMacro(constants, ER_RPL_INFO_DATA_TOO_LONG) < 0) goto error;
#endif
#ifdef ER_NETWORK_READ_EVENT_CHECKSUM_FAILURE
    if (PyModule_AddIntMacro(constants, ER_NETWORK_READ_EVENT_CHECKSUM_FAILURE) < 0) goto error;
#endif
#ifdef ER_BINLOG_READ_EVENT_CHECKSUM_FAILURE
    if (PyModule_AddIntMacro(constants, ER_BINLOG_READ_EVENT_CHECKSUM_FAILURE) < 0) goto error;
#endif
#ifdef ER_BINLOG_STMT_CACHE_SIZE_GREATER_THAN_MAX
    if (PyModule_AddIntMacro(constants, ER_BINLOG_STMT_CACHE_SIZE_GREATER_THAN_MAX) < 0) goto error;
#endif
#ifdef ER_CANT_UPDATE_TABLE_IN_CREATE_TABLE_SELECT
    if (PyModule_AddIntMacro(constants, ER_CANT_UPDATE_TABLE_IN_CREATE_TABLE_SELECT) < 0) goto error;
#endif
#ifdef ER_PARTITION_CLAUSE_ON_NONPARTITIONED
    if (PyModule_AddIntMacro(constants, ER_PARTITION_CLAUSE_ON_NONPARTITIONED) < 0) goto error;
#endif
#ifdef ER_ROW_DOES_NOT_MATCH_GIVEN_PARTITION_SET
    if (PyModule_AddIntMacro(constants, ER_ROW_DOES_NOT_MATCH_GIVEN_PARTITION_SET) < 0) goto error;
#endif
#ifdef ER_NO_SUCH_PARTITION__UNUSED
    if (PyModule_AddIntMacro(constants, ER_NO_SUCH_PARTITION__UNUSED) < 0) goto error;
#endif
#ifdef ER_CHANGE_RPL_INFO_REPOSITORY_FAILURE
    if (PyModule_AddIntMacro(constants, ER_CHANGE_RPL_INFO_REPOSITORY_FAILURE) < 0) goto error;
#endif
#ifdef ER_WARNING_NOT_COMPLETE_ROLLBACK_WITH_CREATED_TEMP_TABLE
    if (PyModule_AddIntMacro(constants, ER_WARNING_NOT_COMPLETE_ROLLBACK_WITH_CREATED_TEMP_TABLE) < 0) goto error;
#endif
#ifdef ER_WARNING_NOT_COMPLETE_ROLLBACK_WITH_DROPPED_TEMP_TABLE
    if (PyModule_AddIntMacro(constants, ER_WARNING_NOT_COMPLETE_ROLLBACK_WITH_DROPPED_TEMP_TABLE) < 0) goto error;
#endif
#ifdef ER_MTS_FEATURE_IS_NOT_SUPPORTED
    if (PyModule_AddIntMacro(constants, ER_MTS_FEATURE_IS_NOT_SUPPORTED) < 0) goto error;
#endif
#ifdef ER_MTS_UPDATED_DBS_GREATER_MAX
    if (PyModule_AddIntMacro(constants, ER_MTS_UPDATED_DBS_GREATER_MAX) < 0) goto error;
#endif
#ifdef ER_MTS_CANT_PARALLEL
    if (PyModule_AddIntMacro(constants, ER_MTS_CANT_PARALLEL) < 0) goto error;
#endif
#ifdef ER_MTS_INCONSISTENT_DATA
    if (PyModule_AddIntMacro(constants, ER_MTS_INCONSISTENT_DATA) < 0) goto error;
#endif
#ifdef ER_FULLTEXT_NOT_SUPPORTED_WITH_PARTITIONING
    if (PyModule_AddIntMacro(constants, ER_FULLTEXT_NOT_SUPPORTED_WITH_PARTITIONING) < 0) goto error;
#endif
#ifdef ER_DA_INVALID_CONDITION_NUMBER
    if (PyModule_AddIntMacro(constants, ER_DA_INVALID_CONDITION_NUMBER) < 0) goto error;
#endif
#ifdef ER_INSECURE_PLAIN_TEXT
    if (PyModule_AddIntMacro(constants, ER_INSECURE_PLAIN_TEXT) < 0) goto error;
#endif
#ifdef ER_INSECURE_CHANGE_MASTER
    if (PyModule_AddIntMacro(constants, ER_INSECURE_CHANGE_MASTER) < 0) goto error;
#endif
#ifdef ER_FOREIGN_DUPLICATE_KEY_WITH_CHILD_INFO
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_DUPLICATE_KEY_WITH_CHILD_INFO) < 0) goto error;
#endif
#ifdef ER_FOREIGN_DUPLICATE_KEY_WITHOUT_CHILD_INFO
    if (PyModule_AddIntMacro(constants, ER_FOREIGN_DUPLICATE_KEY_WITHOUT_CHILD_INFO) < 0) goto error;
#endif
#ifdef ER_SQLTHREAD_WITH_SECURE_SLAVE
    if (PyModule_AddIntMacro(constants, ER_SQLTHREAD_WITH_SECURE_SLAVE) < 0) goto error;
#endif
#ifdef ER_TABLE_HAS_NO_FT
    if (PyModule_AddIntMacro(constants, ER_TABLE_HAS_NO_FT) < 0) goto error;
#endif
#ifdef ER_VARIABLE_NOT_SETTABLE_IN_SF_OR_TRIGGER
    if (PyModule_AddIntMacro(constants, ER_VARIABLE_NOT_SETTABLE_IN_SF_OR_TRIGGER) < 0) goto error;
#endif
#ifdef ER_VARIABLE_NOT_SETTABLE_IN_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_VARIABLE_NOT_SETTABLE_IN_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_GTID_NEXT_IS_NOT_IN_GTID_NEXT_LIST
    if (PyModule_AddIntMacro(constants, ER_GTID_NEXT_IS_NOT_IN_GTID_NEXT_LIST) < 0) goto error;
#endif
#ifdef ER_CANT_CHANGE_GTID_NEXT_IN_TRANSACTION_WHEN_GTID_NEXT_LIST_IS_NULL
    if (PyModule_AddIntMacro(constants, ER_CANT_CHANGE_GTID_NEXT_IN_TRANSACTION_WHEN_GTID_NEXT_LIST_IS_NULL) < 0) goto error;
#endif
#ifdef ER_SET_STATEMENT_CANNOT_INVOKE_FUNCTION
    if (PyModule_AddIntMacro(constants, ER_SET_STATEMENT_CANNOT_INVOKE_FUNCTION) < 0) goto error;
#endif
#ifdef ER_GTID_NEXT_CANT_BE_AUTOMATIC_IF_GTID_NEXT_LIST_IS_NON_NULL
    if (PyModule_AddIntMacro(constants, ER_GTID_NEXT_CANT_BE_AUTOMATIC_IF_GTID_NEXT_LIST_IS_NON_NULL) < 0) goto error;
#endif
#ifdef ER_SKIPPING_LOGGED_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_SKIPPING_LOGGED_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_MALFORMED_GTID_SET_SPECIFICATION
    if (PyModule_AddIntMacro(constants, ER_MALFORMED_GTID_SET_SPECIFICATION) < 0) goto error;
#endif
#ifdef ER_MALFORMED_GTID_SET_ENCODING
    if (PyModule_AddIntMacro(constants, ER_MALFORMED_GTID_SET_ENCODING) < 0) goto error;
#endif
#ifdef ER_MALFORMED_GTID_SPECIFICATION
    if (PyModule_AddIntMacro(constants, ER_MALFORMED_GTID_SPECIFICATION) < 0) goto error;
#endif
#ifdef ER_GNO_EXHAUSTED
    if (PyModule_AddIntMacro(constants, ER_GNO_EXHAUSTED) < 0) goto error;
#endif
#ifdef ER_BAD_SLAVE_AUTO_POSITION
    if (PyModule_AddIntMacro(constants, ER_BAD_SLAVE_AUTO_POSITION) < 0) goto error;
#endif
#ifdef ER_AUTO_POSITION_REQUIRES_GTID_MODE_ON
    if (PyModule_AddIntMacro(constants, ER_AUTO_POSITION_REQUIRES_GTID_MODE_ON) < 0) goto error;
#endif
#ifdef ER_CANT_DO_IMPLICIT_COMMIT_IN_TRX_WHEN_GTID_NEXT_IS_SET
    if (PyModule_AddIntMacro(constants, ER_CANT_DO_IMPLICIT_COMMIT_IN_TRX_WHEN_GTID_NEXT_IS_SET) < 0) goto error;
#endif
#ifdef ER_GTID_MODE_2_OR_3_REQUIRES_ENFORCE_GTID_CONSISTENCY_ON
    if (PyModule_AddIntMacro(constants, ER_GTID_MODE_2_OR_3_REQUIRES_ENFORCE_GTID_CONSISTENCY_ON) < 0) goto error;
#endif
#ifdef ER_GTID_MODE_REQUIRES_BINLOG
    if (PyModule_AddIntMacro(constants, ER_GTID_MODE_REQUIRES_BINLOG) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_NEXT_TO_GTID_WHEN_GTID_MODE_IS_OFF
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_NEXT_TO_GTID_WHEN_GTID_MODE_IS_OFF) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_NEXT_TO_ANONYMOUS_WHEN_GTID_MODE_IS_ON
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_NEXT_TO_ANONYMOUS_WHEN_GTID_MODE_IS_ON) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_NEXT_LIST_TO_NON_NULL_WHEN_GTID_MODE_IS_OFF
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_NEXT_LIST_TO_NON_NULL_WHEN_GTID_MODE_IS_OFF) < 0) goto error;
#endif
#ifdef ER_FOUND_GTID_EVENT_WHEN_GTID_MODE_IS_OFF
    if (PyModule_AddIntMacro(constants, ER_FOUND_GTID_EVENT_WHEN_GTID_MODE_IS_OFF) < 0) goto error;
#endif
#ifdef ER_GTID_UNSAFE_NON_TRANSACTIONAL_TABLE
    if (PyModule_AddIntMacro(constants, ER_GTID_UNSAFE_NON_TRANSACTIONAL_TABLE) < 0) goto error;
#endif
#ifdef ER_GTID_UNSAFE_CREATE_SELECT
    if (PyModule_AddIntMacro(constants, ER_GTID_UNSAFE_CREATE_SELECT) < 0) goto error;
#endif
#ifdef ER_GTID_UNSAFE_CREATE_DROP_TEMPORARY_TABLE_IN_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_GTID_UNSAFE_CREATE_DROP_TEMPORARY_TABLE_IN_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_GTID_MODE_CAN_ONLY_CHANGE_ONE_STEP_AT_A_TIME
    if (PyModule_AddIntMacro(constants, ER_GTID_MODE_CAN_ONLY_CHANGE_ONE_STEP_AT_A_TIME) < 0) goto error;
#endif
#ifdef ER_MASTER_HAS_PURGED_REQUIRED_GTIDS
    if (PyModule_AddIntMacro(constants, ER_MASTER_HAS_PURGED_REQUIRED_GTIDS) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_NEXT_WHEN_OWNING_GTID
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_NEXT_WHEN_OWNING_GTID) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_EXPLAIN_FORMAT
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_EXPLAIN_FORMAT) < 0) goto error;
#endif
#ifdef ER_CANT_EXECUTE_IN_READ_ONLY_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_CANT_EXECUTE_IN_READ_ONLY_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_TOO_LONG_TABLE_PARTITION_COMMENT
    if (PyModule_AddIntMacro(constants, ER_TOO_LONG_TABLE_PARTITION_COMMENT) < 0) goto error;
#endif
#ifdef ER_SLAVE_CONFIGURATION
    if (PyModule_AddIntMacro(constants, ER_SLAVE_CONFIGURATION) < 0) goto error;
#endif
#ifdef ER_INNODB_FT_LIMIT
    if (PyModule_AddIntMacro(constants, ER_INNODB_FT_LIMIT) < 0) goto error;
#endif
#ifdef ER_INNODB_NO_FT_TEMP_TABLE
    if (PyModule_AddIntMacro(constants, ER_INNODB_NO_FT_TEMP_TABLE) < 0) goto error;
#endif
#ifdef ER_INNODB_FT_WRONG_DOCID_COLUMN
    if (PyModule_AddIntMacro(constants, ER_INNODB_FT_WRONG_DOCID_COLUMN) < 0) goto error;
#endif
#ifdef ER_INNODB_FT_WRONG_DOCID_INDEX
    if (PyModule_AddIntMacro(constants, ER_INNODB_FT_WRONG_DOCID_INDEX) < 0) goto error;
#endif
#ifdef ER_INNODB_ONLINE_LOG_TOO_BIG
    if (PyModule_AddIntMacro(constants, ER_INNODB_ONLINE_LOG_TOO_BIG) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_ALTER_ALGORITHM
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_ALTER_ALGORITHM) < 0) goto error;
#endif
#ifdef ER_UNKNOWN_ALTER_LOCK
    if (PyModule_AddIntMacro(constants, ER_UNKNOWN_ALTER_LOCK) < 0) goto error;
#endif
#ifdef ER_MTS_CHANGE_MASTER_CANT_RUN_WITH_GAPS
    if (PyModule_AddIntMacro(constants, ER_MTS_CHANGE_MASTER_CANT_RUN_WITH_GAPS) < 0) goto error;
#endif
#ifdef ER_MTS_RECOVERY_FAILURE
    if (PyModule_AddIntMacro(constants, ER_MTS_RECOVERY_FAILURE) < 0) goto error;
#endif
#ifdef ER_MTS_RESET_WORKERS
    if (PyModule_AddIntMacro(constants, ER_MTS_RESET_WORKERS) < 0) goto error;
#endif
#ifdef ER_COL_COUNT_DOESNT_MATCH_CORRUPTED_V2
    if (PyModule_AddIntMacro(constants, ER_COL_COUNT_DOESNT_MATCH_CORRUPTED_V2) < 0) goto error;
#endif
#ifdef ER_SLAVE_SILENT_RETRY_TRANSACTION
    if (PyModule_AddIntMacro(constants, ER_SLAVE_SILENT_RETRY_TRANSACTION) < 0) goto error;
#endif
#ifdef ER_DISCARD_FK_CHECKS_RUNNING
    if (PyModule_AddIntMacro(constants, ER_DISCARD_FK_CHECKS_RUNNING) < 0) goto error;
#endif
#ifdef ER_TABLE_SCHEMA_MISMATCH
    if (PyModule_AddIntMacro(constants, ER_TABLE_SCHEMA_MISMATCH) < 0) goto error;
#endif
#ifdef ER_TABLE_IN_SYSTEM_TABLESPACE
    if (PyModule_AddIntMacro(constants, ER_TABLE_IN_SYSTEM_TABLESPACE) < 0) goto error;
#endif
#ifdef ER_IO_READ_ERROR
    if (PyModule_AddIntMacro(constants, ER_IO_READ_ERROR) < 0) goto error;
#endif
#ifdef ER_IO_WRITE_ERROR
    if (PyModule_AddIntMacro(constants, ER_IO_WRITE_ERROR) < 0) goto error;
#endif
#ifdef ER_TABLESPACE_MISSING
    if (PyModule_AddIntMacro(constants, ER_TABLESPACE_MISSING) < 0) goto error;
#endif
#ifdef ER_TABLESPACE_EXISTS
    if (PyModule_AddIntMacro(constants, ER_TABLESPACE_EXISTS) < 0) goto error;
#endif
#ifdef ER_TABLESPACE_DISCARDED
    if (PyModule_AddIntMacro(constants, ER_TABLESPACE_DISCARDED) < 0) goto error;
#endif
#ifdef ER_INTERNAL_ERROR
    if (PyModule_AddIntMacro(constants, ER_INTERNAL_ERROR) < 0) goto error;
#endif
#ifdef ER_INNODB_IMPORT_ERROR
    if (PyModule_AddIntMacro(constants, ER_INNODB_IMPORT_ERROR) < 0) goto error;
#endif
#ifdef ER_INNODB_INDEX_CORRUPT
    if (PyModule_AddIntMacro(constants, ER_INNODB_INDEX_CORRUPT) < 0) goto error;
#endif
#ifdef ER_INVALID_YEAR_COLUMN_LENGTH
    if (PyModule_AddIntMacro(constants, ER_INVALID_YEAR_COLUMN_LENGTH) < 0) goto error;
#endif
#ifdef ER_NOT_VALID_PASSWORD
    if (PyModule_AddIntMacro(constants, ER_NOT_VALID_PASSWORD) < 0) goto error;
#endif
#ifdef ER_MUST_CHANGE_PASSWORD
    if (PyModule_AddIntMacro(constants, ER_MUST_CHANGE_PASSWORD) < 0) goto error;
#endif
#ifdef ER_FK_NO_INDEX_CHILD
    if (PyModule_AddIntMacro(constants, ER_FK_NO_INDEX_CHILD) < 0) goto error;
#endif
#ifdef ER_FK_NO_INDEX_PARENT
    if (PyModule_AddIntMacro(constants, ER_FK_NO_INDEX_PARENT) < 0) goto error;
#endif
#ifdef ER_FK_FAIL_ADD_SYSTEM
    if (PyModule_AddIntMacro(constants, ER_FK_FAIL_ADD_SYSTEM) < 0) goto error;
#endif
#ifdef ER_FK_CANNOT_OPEN_PARENT
    if (PyModule_AddIntMacro(constants, ER_FK_CANNOT_OPEN_PARENT) < 0) goto error;
#endif
#ifdef ER_FK_INCORRECT_OPTION
    if (PyModule_AddIntMacro(constants, ER_FK_INCORRECT_OPTION) < 0) goto error;
#endif
#ifdef ER_FK_DUP_NAME
    if (PyModule_AddIntMacro(constants, ER_FK_DUP_NAME) < 0) goto error;
#endif
#ifdef ER_PASSWORD_FORMAT
    if (PyModule_AddIntMacro(constants, ER_PASSWORD_FORMAT) < 0) goto error;
#endif
#ifdef ER_FK_COLUMN_CANNOT_DROP
    if (PyModule_AddIntMacro(constants, ER_FK_COLUMN_CANNOT_DROP) < 0) goto error;
#endif
#ifdef ER_FK_COLUMN_CANNOT_DROP_CHILD
    if (PyModule_AddIntMacro(constants, ER_FK_COLUMN_CANNOT_DROP_CHILD) < 0) goto error;
#endif
#ifdef ER_FK_COLUMN_NOT_NULL
    if (PyModule_AddIntMacro(constants, ER_FK_COLUMN_NOT_NULL) < 0) goto error;
#endif
#ifdef ER_DUP_INDEX
    if (PyModule_AddIntMacro(constants, ER_DUP_INDEX) < 0) goto error;
#endif
#ifdef ER_FK_COLUMN_CANNOT_CHANGE
    if (PyModule_AddIntMacro(constants, ER_FK_COLUMN_CANNOT_CHANGE) < 0) goto error;
#endif
#ifdef ER_FK_COLUMN_CANNOT_CHANGE_CHILD
    if (PyModule_AddIntMacro(constants, ER_FK_COLUMN_CANNOT_CHANGE_CHILD) < 0) goto error;
#endif
#ifdef ER_FK_CANNOT_DELETE_PARENT
    if (PyModule_AddIntMacro(constants, ER_FK_CANNOT_DELETE_PARENT) < 0) goto error;
#endif
#ifdef ER_MALFORMED_PACKET
    if (PyModule_AddIntMacro(constants, ER_MALFORMED_PACKET) < 0) goto error;
#endif
#ifdef ER_READ_ONLY_MODE
    if (PyModule_AddIntMacro(constants, ER_READ_ONLY_MODE) < 0) goto error;
#endif
#ifdef ER_GTID_NEXT_TYPE_UNDEFINED_GROUP
    if (PyModule_AddIntMacro(constants, ER_GTID_NEXT_TYPE_UNDEFINED_GROUP) < 0) goto error;
#endif
#ifdef ER_VARIABLE_NOT_SETTABLE_IN_SP
    if (PyModule_AddIntMacro(constants, ER_VARIABLE_NOT_SETTABLE_IN_SP) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_PURGED_WHEN_GTID_MODE_IS_OFF
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_PURGED_WHEN_GTID_MODE_IS_OFF) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_PURGED_WHEN_GTID_EXECUTED_IS_NOT_EMPTY
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_PURGED_WHEN_GTID_EXECUTED_IS_NOT_EMPTY) < 0) goto error;
#endif
#ifdef ER_CANT_SET_GTID_PURGED_WHEN_OWNED_GTIDS_IS_NOT_EMPTY
    if (PyModule_AddIntMacro(constants, ER_CANT_SET_GTID_PURGED_WHEN_OWNED_GTIDS_IS_NOT_EMPTY) < 0) goto error;
#endif
#ifdef ER_GTID_PURGED_WAS_CHANGED
    if (PyModule_AddIntMacro(constants, ER_GTID_PURGED_WAS_CHANGED) < 0) goto error;
#endif
#ifdef ER_GTID_EXECUTED_WAS_CHANGED
    if (PyModule_AddIntMacro(constants, ER_GTID_EXECUTED_WAS_CHANGED) < 0) goto error;
#endif
#ifdef ER_BINLOG_STMT_MODE_AND_NO_REPL_TABLES
    if (PyModule_AddIntMacro(constants, ER_BINLOG_STMT_MODE_AND_NO_REPL_TABLES) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_COPY
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_COPY) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_PARTITION
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_PARTITION) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_FK_RENAME
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_FK_RENAME) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_COLUMN_TYPE
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_COLUMN_TYPE) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_FK_CHECK
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_FK_CHECK) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_IGNORE
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_IGNORE) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_NOPK
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_NOPK) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_AUTOINC
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_AUTOINC) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_HIDDEN_FTS
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_HIDDEN_FTS) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_CHANGE_FTS
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_CHANGE_FTS) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_FTS
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_FTS) < 0) goto error;
#endif
#ifdef ER_SQL_SLAVE_SKIP_COUNTER_NOT_SETTABLE_IN_GTID_MODE
    if (PyModule_AddIntMacro(constants, ER_SQL_SLAVE_SKIP_COUNTER_NOT_SETTABLE_IN_GTID_MODE) < 0) goto error;
#endif
#ifdef ER_DUP_UNKNOWN_IN_INDEX
    if (PyModule_AddIntMacro(constants, ER_DUP_UNKNOWN_IN_INDEX) < 0) goto error;
#endif
#ifdef ER_IDENT_CAUSES_TOO_LONG_PATH
    if (PyModule_AddIntMacro(constants, ER_IDENT_CAUSES_TOO_LONG_PATH) < 0) goto error;
#endif
#ifdef ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_NOT_NULL
    if (PyModule_AddIntMacro(constants, ER_ALTER_OPERATION_NOT_SUPPORTED_REASON_NOT_NULL) < 0) goto error;
#endif
#ifdef ER_MUST_CHANGE_PASSWORD_LOGIN
    if (PyModule_AddIntMacro(constants, ER_MUST_CHANGE_PASSWORD_LOGIN) < 0) goto error;
#endif
#ifdef ER_ROW_IN_WRONG_PARTITION
    if (PyModule_AddIntMacro(constants, ER_ROW_IN_WRONG_PARTITION) < 0) goto error;
#endif
#ifdef ER_MTS_EVENT_BIGGER_PENDING_JOBS_SIZE_MAX
    if (PyModule_AddIntMacro(constants, ER_MTS_EVENT_BIGGER_PENDING_JOBS_SIZE_MAX) < 0) goto error;
#endif
#ifdef ER_INNODB_NO_FT_USES_PARSER
    if (PyModule_AddIntMacro(constants, ER_INNODB_NO_FT_USES_PARSER) < 0) goto error;
#endif
#ifdef ER_BINLOG_LOGICAL_CORRUPTION
    if (PyModule_AddIntMacro(constants, ER_BINLOG_LOGICAL_CORRUPTION) < 0) goto error;
#endif
#ifdef ER_WARN_PURGE_LOG_IN_USE
    if (PyModule_AddIntMacro(constants, ER_WARN_PURGE_LOG_IN_USE) < 0) goto error;
#endif
#ifdef ER_WARN_PURGE_LOG_IS_ACTIVE
    if (PyModule_AddIntMacro(constants, ER_WARN_PURGE_LOG_IS_ACTIVE) < 0) goto error;
#endif
#ifdef ER_AUTO_INCREMENT_CONFLICT
    if (PyModule_AddIntMacro(constants, ER_AUTO_INCREMENT_CONFLICT) < 0) goto error;
#endif
#ifdef WARN_ON_BLOCKHOLE_IN_RBR
    if (PyModule_AddIntMacro(constants, WARN_ON_BLOCKHOLE_IN_RBR) < 0) goto error;
#endif
#ifdef ER_SLAVE_MI_INIT_REPOSITORY
    if (PyModule_AddIntMacro(constants, ER_SLAVE_MI_INIT_REPOSITORY) < 0) goto error;
#endif
#ifdef ER_SLAVE_RLI_INIT_REPOSITORY
    if (PyModule_AddIntMacro(constants, ER_SLAVE_RLI_INIT_REPOSITORY) < 0) goto error;
#endif
#ifdef ER_ACCESS_DENIED_CHANGE_USER_ERROR
    if (PyModule_AddIntMacro(constants, ER_ACCESS_DENIED_CHANGE_USER_ERROR) < 0) goto error;
#endif
#ifdef ER_INNODB_READ_ONLY
    if (PyModule_AddIntMacro(constants, ER_INNODB_READ_ONLY) < 0) goto error;
#endif
#ifdef ER_STOP_SLAVE_SQL_THREAD_TIMEOUT
    if (PyModule_AddIntMacro(constants, ER_STOP_SLAVE_SQL_THREAD_TIMEOUT) < 0) goto error;
#endif
#ifdef ER_STOP_SLAVE_IO_THREAD_TIMEOUT
    if (PyModule_AddIntMacro(constants, ER_STOP_SLAVE_IO_THREAD_TIMEOUT) < 0) goto error;
#endif
#ifdef ER_TABLE_CORRUPT
    if (PyModule_AddIntMacro(constants, ER_TABLE_CORRUPT) < 0) goto error;
#endif
#ifdef ER_TEMP_FILE_WRITE_FAILURE
    if (PyModule_AddIntMacro(constants, ER_TEMP_FILE_WRITE_FAILURE) < 0) goto error;
#endif
#ifdef ER_INNODB_FT_AUX_NOT_HEX_ID
    if (PyModule_AddIntMacro(constants, ER_INNODB_FT_AUX_NOT_HEX_ID) < 0) goto error;
#endif
#ifdef ER_OLD_TEMPORALS_UPGRADED
    if (PyModule_AddIntMacro(constants, ER_OLD_TEMPORALS_UPGRADED) < 0) goto error;
#endif
#ifdef ER_INNODB_FORCED_RECOVERY
    if (PyModule_AddIntMacro(constants, ER_INNODB_FORCED_RECOVERY) < 0) goto error;
#endif
#ifdef ER_AES_INVALID_IV
    if (PyModule_AddIntMacro(constants, ER_AES_INVALID_IV) < 0) goto error;
#endif
#ifdef ER_ERROR_LAST
    if (PyModule_AddIntMacro(constants, ER_ERROR_LAST) < 0) goto error;
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
