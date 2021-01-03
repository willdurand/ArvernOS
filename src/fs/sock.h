/** @file */
#ifndef FS_SOCK_H
#define FS_SOCK_H

#include <fs/vfs.h>
#include <stdbool.h>

#define FS_SOCK_MOUNTPOINT "/proc/net"

#define MAX_ENTRIES   10
#define ENTRY_BUF_LEN 2048

typedef struct sock_entry
{
  inode_t inode;
  // circular buffer
  uint16_t len;
  uint16_t read;
  uint16_t write;
  uint8_t buffer[ENTRY_BUF_LEN];
} sock_entry_t;

/**
 * Initializes the "sock" file system.
 */
bool sock_fs_init();

/**
 * Creates the "sock" file system root node.
 *
 * @return the root inode of the sock file system
 */
inode_t sock_fs_create();

#endif
