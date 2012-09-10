#ifndef MYSQL_PROTOCOL_H
#define MYSQL_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

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
