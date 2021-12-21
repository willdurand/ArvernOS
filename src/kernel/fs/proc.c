#include <fs/proc.h>

#include <arvern/utils.h>
#include <fs/logging.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024

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

bool proc_fs_init()
{
  return (bool)vfs_mount(FS_PROC_MOUNTPOINT, proc_fs_create());
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

uint64_t proc_read(inode_t inode, void* buffer, uint64_t size, uint64_t offset)
{
  if (vfs_type(inode) != FS_FILE) {
    return 0;
  }

  // Empty buffer.
  strcpy(buffer, "");

  if (inode->data) {
    char buf[BUFSIZE];

    if (((proc_read_t*)(inode->data))(buf, BUFSIZE) != 0) {
      return 0;
    }

    uint64_t len = strlen(buf);

    if (offset > len) {
      return 0;
    }

    if (size > len) {
      size = len;
    }

    if (offset + size > len) {
      size = len - offset;
    }

    memcpy(buffer, (char*)buf + offset, size);
    ((char*)buffer)[size] = '\0';

    return size;
  }

  return 0;
}
