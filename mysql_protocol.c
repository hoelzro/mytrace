#include "mysql_protocol.h"

#include <string.h>

#define MYSQL_PROTO_VERSION 10

static uint16_t
read_short(uint8_t *buffer)
{
    return (buffer[1] << 8) | buffer[0];
}

static uint32_t
read_long(uint8_t *buffer)
{
    return (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
}

/* XXX
 *
 * This routine currently assumes that the entire first part of the handshake
 * is done in a single read()/recv().  It would be nice if we were more accomodating;
 * however, libmysqlclient does it in a single read, so I'm not going to worry about it...
 * for now.
 */

/* XXX we also don't check the size; I think that's ok for now */
int
read_mysql_handshake(uint8_t *buffer, size_t size, struct mysql_handshake_packet *packet)
{
    uint32_t packet_size;

    packet_size = read_long(buffer);
    packet_size &= 0xffffff; /* the fourth byte is the sequence ID, so clear it */

    if(packet_size != size - 4) { /* four bytes for the header */
        return 0;
    }
    /* we don't worry about the sequence ID; we assume it's correct */

    buffer += 4;
    size   -= 4;

    packet->proto_version = buffer[0];

    if(packet->proto_version != MYSQL_PROTO_VERSION) {
        return 0;
    }

    ++buffer;
    --size;

    if(strnlen(buffer, size) == size) { /* no NULL-terminated string found */
        return 0;
    }
    packet->server_version = buffer;

    buffer += strlen(packet->server_version) + 1;
    size   -= strlen(packet->server_version) + 1;

    packet->connection_id = buffer[3];
    packet->connection_id <<= 8;
    packet->connection_id += buffer[2];
    packet->connection_id <<= 8;
    packet->connection_id += buffer[1];
    packet->connection_id <<= 8;
    packet->connection_id += buffer[0];

    buffer += 4;
    size   -= 4;

    memcpy(packet->auth_plugin_data, buffer, 8);

    buffer += 8;
    size   -= 8;

    /* there's a filler byte in there */
    ++buffer;
    --size;

    packet->capability_flags = buffer[1];
    packet->capability_flags <<= 8;
    packet->capability_flags += buffer[0];

    buffer += 2;
    size   -= 2;

    packet->charset = buffer[0];

    ++buffer;
    --size;

    packet->status_flags = buffer[1];
    packet->status_flags <<= 8;
    packet->status_flags += buffer[0];

    buffer += 2;
    size   -= 2;

    unsigned long upper_caps = 0;
    upper_caps = buffer[1];
    upper_caps <<= 8;
    upper_caps += buffer[0];
    upper_caps <<= 16;

    packet->capability_flags |= upper_caps;

    buffer += 2;
    size   -= 2;

    return 1;
}
