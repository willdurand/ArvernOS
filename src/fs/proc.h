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
 * This function is used to de-initialize the "proc" file system.
 */
void proc_fs_deinit();

#endif
