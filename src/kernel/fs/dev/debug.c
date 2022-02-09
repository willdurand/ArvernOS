#include <fs/dev.h>

#include <arvern/utils.h>
#include <initcall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

inode_t debug_create();
uint64_t debug_stat(inode_t node, vfs_stat_t* st);
uint64_t debug_write(inode_t node,
                     void* buffer,
                     uint64_t size,
                     uint64_t offset);
uint64_t debug_isatty(inode_t inode);

static vfs_driver_t debug_driver = {
  .open = NULL,
  .close = NULL,
  .read = NULL,
  .write = debug_write,
  .stat = debug_stat,
  .isatty = debug_isatty,
  .readdir = NULL,
  .finddir = NULL,
  .cleanup = NULL,
  .create = NULL,
};

inode_t debug_create()
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "debug");
  node->driver = &debug_driver;
  node->type = FS_CHARDEVICE;

  return node;
}

uint64_t debug_stat(inode_t inode, vfs_stat_t* st)
{
  UNUSED(inode);

  st->mode |= S_IFCHR;

  return 0;
}

uint64_t debug_write(inode_t inode,
                     void* buffer,
                     uint64_t size,
                     uint64_t offset)
{
  UNUSED(inode);

  if (offset < size) {
    printf("%s", (char*)buffer + offset);
    return size - offset;
  }

  return 0;
}

uint64_t debug_isatty(inode_t inode)
{
  UNUSED(inode);

  return 1;
}

int dev_debug_init()
{
  if (!vfs_mount(FS_DEV_MOUNTPOINT "/debug", debug_create())) {
    return 1;
  }

  return 0;
}

initcall_fs_register(dev_debug_init);
