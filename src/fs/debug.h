/** @file */
#ifndef FS_DEBUG_H
#define FS_DEBUG_H

#include <fs/vfs.h>
#include <stdint.h>

#define FS_DEBUG_MOUNTPOINT "/debug"

/**
 * @brief Creates a "debug" file system.
 *
 * This "debug" file system is based on Thomas Lovén's work:
 * http://thomasloven.com/blog/2013/12/Debug-Filesystem/. The idea is to have a
 * very simple file system to test the virtual file system implementation. It
 * is only possible to write to this file system, and doing this will output
 * the content to the screen.
 *
 * @return the "root" inode of the debug file system
 */
inode_t debug_fs_init();

#endif
