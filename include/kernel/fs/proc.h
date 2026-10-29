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

typedef int proc_read_t(char* buf, size_t size, uint64_t offset);

/**
 * Initializes the "proc" file system.
 *
 * The "proc" file system exposes kernel information using files.
 *
 * @return 0 on success
 */
int proc_fs_init();

// This function is used to create a procfs pseudo-file. It must be publicly
// accessible but should generally NOT be used directly. Instead, use
// `proc_fs_register_readonly_file()`.
inode_t proc_fs_create_readonly_file(const char* name, proc_read_t* read_fn);

/**
 * Copies a slice of the source string into the destination buffer.
 *
 * The slice starts at `offset`. At most `size - 1` bytes are copied and the
 * result is always NUL-terminated. If `offset` is past the end of the source
 * string, the destination buffer is set to an empty string.
 *
 * @param buf destination buffer
 * @param size size of the destination buffer in bytes
 * @param offset byte offset into the source string
 * @param src source string
 * @param len length of the source string in bytes
 * @return the number of bytes copied
 */
size_t proc_copy_out(char* buf,
                     size_t size,
                     uint64_t offset,
                     const char* src,
                     size_t len);

#ifdef TEST_ENV
inode_t proc_fs_create();
#endif

#endif
