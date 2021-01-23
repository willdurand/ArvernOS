/** @file */
#ifndef FS_DEBUG_H
#define FS_DEBUG_H

#include <fs/vfs.h>
#include <stdbool.h>

#define FS_DEBUG_MOUNTPOINT "/dev/debug"

/**
 * Initializes the "debug" file system.
 *
 * This "debug" file system is based on Thomas Lovén's work:
 * http://thomasloven.com/blog/2013/12/Debug-Filesystem/. The idea is to have a
 * very simple file system to test the virtual file system implementation. It
 * is only possible to write to this file system, and doing this will output
 * the content to the screen.
 */
bool debug_fs_init();

/**
 * Creates the "debug" file system root node.
 *
 * @return the root inode of the debug file system
 */
inode_t debug_fs_create();

#endif
