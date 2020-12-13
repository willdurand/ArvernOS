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
  inode_t inode;
  uint32_t offset;
  uint32_t flags;
} descriptor_t;

/**
 * Creates a file descriptor.
 *
 * @param inode the inode for the file descriptor to create
 * @param flags some descriptor flags
 * @return a file descriptor ID or -1 in case of an error
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

#endif
