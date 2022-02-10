/** @file */
#ifndef PROC_DESCRIPTOR_H
#define PROC_DESCRIPTOR_H

#include <arpa/inet.h>
#include <fs/vfs.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

typedef struct descriptor
{
  bool used;
  inode_t inode;
  // for file descriptors
  off_t offset;
  uint32_t flags;
  // for socket descriptors
  uint32_t domain;
  uint32_t type;
  uint32_t protocol;
  uint16_t port;
  struct sockaddr_in addr;
  socklen_t addr_len;
} descriptor_t;

/**
 * Creates a file descriptor.
 *
 * @param inode the inode for the file descriptor to create
 * @param flags some descriptor flags
 * @return a descriptor ID or -1 in case of an error
 */
int create_file_descriptor(inode_t inode, uint32_t flags);

/**
 * Creates a socket descriptor.
 *
 * @param inode the inode for the socket descriptor to create
 * @param domain the domain of the socket
 * @param type the type of the socket
 * @param protocol the protocol of the socket
 * @return a descriptor ID or -1 in case of an error
 */
int create_socket_descriptor(inode_t inode,
                             uint32_t domain,
                             uint32_t type,
                             uint32_t protocol);

/**
 * Returns a descriptor.
 *
 * @param id the descriptor ID
 * @return a descriptor or `NULL`
 */
descriptor_t* get_descriptor(int id);

/**
 * Deletes a descriptor.
 *
 * @param id the descriptor ID
 */
void delete_descriptor(int id);

/**
 * Returns a socket descriptor for UDP.
 *
 * @param port the port bound to the socket to return
 * @return the socket descriptor or -1 when not found
 */
int descriptor_udp_lookup(uint16_t port);

/**
 * Returns whether a socket protocol is supported.
 *
 * TODO: move this function somewhere else.
 *
 * @param type socket type
 * @param protocol socket protocol
 * @return `true` when the protocol is supported, `false` otherwise
 */
bool is_protocol_supported(uint32_t type, uint32_t protocol);

void duplicate_descriptor(int oldfd, int newfd);

int descriptor_raw_lookup(uint32_t protocol, in_addr_t src_addr);

#endif
