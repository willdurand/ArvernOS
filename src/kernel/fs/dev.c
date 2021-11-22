#include <fs/dev.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool dev_fs_init()
{
  return (bool)vfs_mount(FS_DEV_MOUNTPOINT, vfs_make_directory("dev"));
}

inode_t dev_fs_create(const char* name, vfs_driver_t* driver)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, name);
  node->driver = driver;
  node->type = FS_CHARDEVICE;

  return node;
}
