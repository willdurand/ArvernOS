#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t debug_stat(inode_t node, struct stat* st);
uint64_t debug_write(inode_t node,
                     void* buffer,
                     uint64_t size,
                     uint64_t offset);
uint64_t debug_isatty(inode_t inode);

vfs_driver_t debug_driver = {
  0,            // open
  0,            // close
  0,            // read
  debug_write,  // write
  debug_stat,   // stat
  debug_isatty, // isatty
  0,            // readdir
  0             // finddir
};

inode_t debug_fs_init()
{
  inode_t node = malloc(sizeof(vfs_node_t));

  strcpy(node->name, "debug");
  node->driver = &debug_driver;
  node->type = FS_CHARDEVICE;

  return node;
}

uint64_t debug_stat(inode_t inode, stat_t* st)
{
  memset(st, 0, sizeof(stat_t));
  st->size = 0;
  return 0;
}

uint64_t debug_write(inode_t inode,
                     void* buffer,
                     uint64_t size,
                     uint64_t offset)
{
  if (offset < size) {
    printf("%s", &buffer[offset]);
    return size - offset;
  }

  return 0;
}

uint64_t debug_isatty(inode_t inode)
{
  return 1;
}
