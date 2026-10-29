#include <fs/proc.h>

#include <arvern/utils.h>
#include <fs/logging.h>
#include <initcall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inode_t proc_fs_create();
uint64_t proc_read(inode_t inode, void* buffer, uint64_t size, uint64_t offset);

static vfs_driver_t proc_driver = {
  .open = NULL,
  .close = NULL,
  .read = proc_read,
  .write = NULL,
  .stat = NULL,
  .isatty = NULL,
  .readdir = NULL,
  .finddir = NULL,
  .cleanup = NULL,
  .create = NULL,
};

int proc_fs_init()
{
  INFO("%s", "fs: mount procfs");

  if (!vfs_mount(FS_PROC_MOUNTPOINT, proc_fs_create())) {
    WARN("%s", "fs: failed to mount procfs");
    return -1;
  }

  return 0;
}

inode_t proc_fs_create()
{
  inode_t node = vfs_make_directory("proc");
  node->driver = &proc_driver;

  return node;
}

inode_t proc_fs_create_readonly_file(const char* name, proc_read_t* read_fn)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, name);
  node->type = FS_FILE;
  node->data = (uintptr_t)read_fn;
  // TODO: We probably want a specific driver for read-only pseudo-files.
  node->driver = &proc_driver;

  return node;
}

size_t proc_copy_out(char* buf,
                     size_t size,
                     uint64_t offset,
                     const char* src,
                     size_t len)
{
  if (size == 0) {
    return 0;
  }

  if (offset >= len) {
    buf[0] = '\0';
    return 0;
  }

  size_t count = len - (size_t)offset;
  if (count >= size) {
    count = size - 1;
  }

  memcpy(buf, src + (size_t)offset, count);
  buf[count] = '\0';

  return count;
}

uint64_t proc_read(inode_t inode, void* buffer, uint64_t size, uint64_t offset)
{
  if (vfs_type(inode) != FS_FILE) {
    return 0;
  }

  if (size == 0 || buffer == NULL) {
    return 0;
  }

  // Empty buffer.
  ((char*)buffer)[0] = '\0';

  if (inode->data) {
    if (((proc_read_t*)(inode->data))(buffer, size, offset) != 0) {
      ((char*)buffer)[0] = '\0';
      return 0;
    }

    return strlen(buffer);
  }

  return 0;
}

initcall_fs_register_with_priority(proc_fs_init, 4);
