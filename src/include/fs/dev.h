/** @file */
#ifndef FS_DEV_H
#define FS_DEV_H

#include <fs/vfs.h>
#include <stdbool.h>
#include <sys/types.h>

/**
 * Initializes the "dev" filesystem.
 *
 * This FS is a bit different than the other existing ones as it creates a set
 * of character devices instead of managing a single file or directory.
 */
bool dev_fs_init();

#endif
