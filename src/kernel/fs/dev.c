#include <fs/dev.h>

#include <core/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inode_t dev_device_create(const char* name, vfs_driver_t* driver);
uint64_t null_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
uint64_t null_write(inode_t node, void* buffer, uint64_t size, uint64_t offset);

static vfs_driver_t dev_null_driver = {
  NULL,       // open
  NULL,       // close
  null_read,  // read
  null_write, // write
  NULL,       // stat
  NULL,       // isatty
  NULL,       // readdir
  NULL,       // finddir
  NULL,       // cleanup
  NULL,       // create
};

bool dev_fs_init()
{
  if (!vfs_mount("/dev", vfs_make_directory("dev"))) {
    return false;
  }

  if (!vfs_mount("/dev/null", dev_device_create("null", &dev_null_driver))) {
    return false;
  }

  return true;
}

inode_t dev_device_create(const char* name, vfs_driver_t* driver)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, name);
  node->driver = driver;
  node->type = FS_CHARDEVICE;

  return node;
}

uint64_t null_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  UNUSED(node);
  UNUSED(*buffer);
  UNUSED(size);
  UNUSED(offset);

  return 0;
}

uint64_t null_write(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  UNUSED(node);
  UNUSED(*buffer);
  UNUSED(offset);

  return size;
}
