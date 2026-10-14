#include <fs/dev.h>

#include <fs/logging.h>
#include <initcall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dev_fs_init()
{
  INFO("%s", "fs: mount devfs");

  if (!vfs_mount(FS_DEV_MOUNTPOINT, vfs_make_directory("dev"))) {
    WARN("%s", "fs: failed to mount devfs");
    return -1;
  }

  return 0;
}

inode_t dev_fs_create(const char* name, vfs_driver_t* driver)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, name);
  node->driver = driver;
  node->type = FS_CHARDEVICE;

  return node;
}

initcall_fs_register_with_priority(dev_fs_init, 3);
