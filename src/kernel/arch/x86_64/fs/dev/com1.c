#include <fs/dev.h>

#include <arvern/utils.h>
#include <drivers/serial.h>
#include <initcall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

inode_t dev_serial_create(uint16_t serial_port);
uint64_t dev_serial_stat(inode_t node, vfs_stat_t* st);
uint64_t dev_serial_write(inode_t node,
                          void* buffer,
                          uint64_t size,
                          uint64_t offset);
uint64_t dev_serial_isatty(inode_t inode);

static vfs_driver_t serial_driver = {
  .open = NULL,
  .close = NULL,
  .read = NULL,
  .write = dev_serial_write,
  .stat = dev_serial_stat,
  .isatty = dev_serial_isatty,
  .readdir = NULL,
  .finddir = NULL,
  .cleanup = NULL,
  .create = NULL,
};

inode_t dev_serial_create(uint16_t serial_port)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "serial");
  node->driver = &serial_driver;
  node->type = FS_CHARDEVICE;
  node->data = serial_port;

  return node;
}

uint64_t dev_serial_stat(inode_t inode, vfs_stat_t* st)
{
  UNUSED(inode);

  st->mode |= S_IFCHR;

  return 0;
}

uint64_t dev_serial_write(inode_t inode,
                          void* buffer,
                          uint64_t size,
                          uint64_t offset)
{
  if (offset < size) {
    serial_print((uint16_t)inode->data, (char*)buffer + offset);
    return size - offset;
  }

  return 0;
}

uint64_t dev_serial_isatty(inode_t inode)
{
  UNUSED(inode);

  return 1;
}

int dev_serial_com1_init()
{
  if (!vfs_mount(FS_DEV_MOUNTPOINT "/com1", dev_serial_create(SERIAL_COM1))) {
    return 1;
  }

  return 0;
}

initcall_fs_register(dev_serial_com1_init);
