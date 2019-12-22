#ifndef FS_DEBUG_H
#define FS_DEBUG_H

#include <fs/vfs.h>
#include <stdint.h>

#define FS_DEBUG_FILENAME "/debug"

inode_t debug_fs_init();
uint64_t debug_stat(inode_t node, struct stat* st);
uint64_t debug_isatty(inode_t node);
uint64_t debug_write(inode_t node, void* buffer, uint64_t size, uint64_t offset);

#endif
