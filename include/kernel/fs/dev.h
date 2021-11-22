/** @file */
#ifndef FS_DEV_H
#define FS_DEV_H

#include <fs/vfs.h>
#include <stdbool.h>
#include <sys/types.h>

#define FS_DEV_MOUNTPOINT "/dev"

/**
 * Initializes the "dev" filesystem.
 */
bool dev_fs_init();

/**
 * Creates a new devfs character device.
 *
 * @param name the name of the character device
 * @param driver the driver of the character device
 * @return the inode of the pseudo-file
 */
inode_t dev_fs_create(const char* name, vfs_driver_t* driver);

#endif
