#include "serial.h"
#include <drivers/serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

uint64_t fs_serial_stat(inode_t node, vfs_stat_t* st);
uint64_t fs_serial_write(inode_t node,
                         void* buffer,
                         uint64_t size,
                         uint64_t offset);
uint64_t fs_serial_isatty(inode_t inode);

static vfs_driver_t serial_driver = {
  NULL,             // open
  NULL,             // close
  NULL,             // read
  fs_serial_write,  // write
  fs_serial_stat,   // stat
  fs_serial_isatty, // isatty
  NULL,             // readdir
  NULL,             // finddir
  NULL,             // cleanup
  NULL,             // create
};

bool serial_fs_init()
{
  if (!vfs_mount(FS_SERIAL_MOUNTPOINT_PATH FS_SERIAL_COM1,
                 serial_device_create(SERIAL_COM1))) {
    return false;
  }

  return true;
}

inode_t serial_device_create(uint16_t serial_port)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "serial");
  node->driver = &serial_driver;
  node->type = FS_CHARDEVICE;
  node->data = serial_port;

  return node;
}

uint64_t fs_serial_stat(inode_t inode, vfs_stat_t* st)
{
  st->mode |= S_IFCHR;

  return 0;
}

uint64_t fs_serial_write(inode_t inode,
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

uint64_t fs_serial_isatty(inode_t inode)
{
  return 1;
}
