#include <fs/dev.h>
#include <init.h>

#include <arvern/utils.h>

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

int dev_null_init()
{
  if (!vfs_mount(FS_DEV_MOUNTPOINT "/null",
                 dev_fs_create("null", &dev_null_driver))) {
    return 1;
  }

  return 0;
}

init_register(dev_null_init);
