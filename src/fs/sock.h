/** @file */
#ifndef FS_SOCK_H
#define FS_SOCK_H

#include <fs/vfs.h>

#define FS_SOCK_MOUNTPOINT "/proc/net"

/**
 * @brief Creates a "sock" file system.
 *
 * @return the "root" inode of the sock file system
 */
inode_t sock_fs_init();

#endif
