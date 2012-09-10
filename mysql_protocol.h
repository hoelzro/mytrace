#ifndef MYSQL_PROTOCOL_H
#define MYSQL_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

enum mysql_commands {
    COM_SLEEP = 0,
    COM_QUIT,
    COM_INIT_DB,
    COM_QUERY,
    COM_FIELD_LIST,
    COM_CREATE_DB,
    COM_DROP_DB,
    COM_REFRESH,
    COM_SHUTDOWN,
    COM_STATISTICS,
    COM_PROCESS_INFO,
    COM_CONNECT,
    COM_PROCESS_KILL,
    COM_DEBUG,
    COM_PING,
    COM_TIME,
    COM_DELAYED_INSERT,
    COM_CHANGE_USER,
    COM_BINLOG_DUMP,
    COM_TABLE_DUMP,
    COM_CONNECT_OUT,
    COM_REGISTER_SLAVE,
    COM_STMT_PREPARE,
    COM_STMT_EXECUTE,
    COM_STMT_SEND_LONG_DATA,
    COM_STMT_CLOSE,
    COM_STMT_RESET,
    COM_SET_OPTION,
    COM_STMT_FETCH,
    COM_DAEMON,
    COM_BINLOG_DUMP_GTID
};

/* uhhh...this is the handshake packet. */
struct mysql_handshake_packet {
    uint8_t     proto_version;
    char       *server_version;
    uint32_t    connection_id;
    char        auth_plugin_data[8];
    uint8_t     charset;
    uint16_t    status_flags;
    uint32_t    capability_flags;
};

int read_mysql_handshake(uint8_t *buffer, size_t size, struct mysql_handshake_packet *packet);

#endif
