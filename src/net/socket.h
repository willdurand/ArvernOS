/** @file */
#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <stdbool.h>
#include <sys/types.h>

/// The size of the circular buffer for a socket.
#define BUFFER_LEN 4096

/// This structure is a per-socket circular buffer.
typedef struct socket_buf
{
  int sockfd;
  uint16_t len;
  uint16_t read;
  uint16_t write;
  uint8_t buffer[BUFFER_LEN];
} socket_buf_t;

/**
 * Appends data to a socket buffer.
 *
 * @param sockfd a socket descriptor ID
 * @param data the data to append
 * @param len the size of the data to append
 */
void socket_bufferize(int sockfd, uint8_t* data, uint16_t len);

/**
 * Reads data from a socket buffer.
 *
 * @param sockfd a socket descriptor ID
 * @param buf the buffer that should receive data
 * @param len the size of data to unbufferize
 * @return the size of data actually unbufferized or `0` when socket buffer is
 * not found
 */
size_t socket_unbufferize(int sockfd, uint8_t* buf, size_t len);

/**
 * Deletes a socket buffer.
 *
 * @param sockfd a socket descriptor ID
 * @return 0 for success, -1 otherwise
 */
int socket_delete_buffer(int sockfd);

/**
 * Returns whether a socket protocol is supported.
 *
 * @param type socket type
 * @param protocol socket protocol
 * @return `true` when the protocol is supported, `false` otherwise
 */
bool is_protocol_supported(int type, int protocol);

#endif
