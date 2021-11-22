/** @file */
#ifndef FS_PROC_H
#define FS_PROC_H

#include <fs/vfs.h>
#include <stdbool.h>
#include <stddef.h>

#define FS_PROC_MOUNTPOINT "/proc"

#define proc_fs_register_readonly_file(name, read_fn)                          \
  vfs_mount(FS_PROC_MOUNTPOINT "/" name,                                       \
            proc_fs_create_readonly_file(name, read_fn));

typedef int proc_read_t(char* buf, size_t size);

/**
 * Initializes the "proc" file system.
 *
 * The "proc" file system exposes kernel information using files.
 */
bool proc_fs_init();

// This function is used to create a procfs pseudo-file. It must be publicly
// accessible but should generally NOT be used directly. Instead, use
// `proc_fs_register_readonly_file()`.
inode_t proc_fs_create_readonly_file(const char* name, proc_read_t* read_fn);

#ifdef TEST_ENV
inode_t proc_fs_create();
#endif

#endif
