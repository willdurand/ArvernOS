/** @file */
#ifndef PROC_DESCRIPTOR_H
#define PROC_DESCRIPTOR_H

#include <fs/vfs.h>
#include <stdbool.h>
#include <stdint.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

typedef struct descriptor
{
  bool used;
  // file descriptor
  inode_t inode;
  uint32_t offset;
  uint32_t flags;
  // socket descriptor
  uint32_t domain;
  uint32_t type;
  uint32_t protocol;
  uint16_t port;
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
 * Creates a socket descriptor.
 *
 * @param domain the domain of the socket
 * @param type the type of the socket
 * @param protocol the protocol of the socket
 * @return a descriptor ID or -1 in case of an error
 */
int create_socket_descriptor(int domain, int type, int protocol);

/**
 * Returns a socket descriptor bound to a given port.
 *
 * @param port the port bound to the socket to return
 * @return the socket descriptor or -1 when not found
 */
int get_socket_descriptor_for_port(uint16_t port);

#endif
