#ifndef FS_PROC_H
#define FS_PROC_H

#include <fs/vfs.h>
#include <stdbool.h>

#define FS_PROC_MOUNTPOINT "/proc"

/**
 * Initializes the "proc" file system.
 *
 * The "proc" file system exposes kernel information using files.
 */
bool proc_fs_init();

/**
 * Creates the "proc" file system root node.
 *
 * @return the root inode of the proc file system
 */
inode_t proc_fs_create();

/**
 * This function is used to update the hostname.
 *
 * @param new_hostname the new hostname
 * @param length the length of the new hostname
 * @return the length of the updated hostname
 */
uint64_t proc_update_hostname(char* new_hostname, uint64_t length);

#endif
