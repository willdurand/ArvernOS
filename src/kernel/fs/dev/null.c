#include <fs/dev.h>

#include <arvern/utils.h>
#include <initcall.h>

uint64_t null_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
uint64_t null_write(inode_t node, void* buffer, uint64_t size, uint64_t offset);

static vfs_driver_t dev_null_driver = {
  .open = NULL,
  .close = NULL,
  .read = null_read,
  .write = null_write,
  .stat = NULL,
  .isatty = NULL,
  .readdir = NULL,
  .finddir = NULL,
  .cleanup = NULL,
  .create = NULL,
};

uint64_t null_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  UNUSED(node);
  UNUSED(buffer);
  UNUSED(size);
  UNUSED(offset);

  return 0;
}

uint64_t null_write(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  UNUSED(node);
  UNUSED(buffer);
  UNUSED(offset);

  return size;
}

int dev_null_init()
{
  if (!vfs_mount(FS_DEV_MOUNTPOINT "/null",
                 dev_fs_create("null", &dev_null_driver))) {
    return 1;
  }

  return 0;
}

initcall_fs_register(dev_null_init);
