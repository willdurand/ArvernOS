/** @file */
#ifndef FS_PROC_H
#define FS_PROC_H

#include <fs/vfs.h>

#define FS_PROC_MOUNTPOINT "/proc"

/**
 * @brief Creates a "proc" file system.
 *
 * This "proc" file system exposes kernel information using files.
 *
 * @return the "root" inode of the proc file system
 */
inode_t proc_fs_init();

/**
 * This function is used to update the hostname.
 *
 * @param new_hostname the new hostname
 * @param length the length of the new hostname
 * @return the length of the updated hostname
 */
uint64_t proc_update_hostname(char* new_hostname, uint64_t length);

#endif
