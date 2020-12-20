#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

uint64_t debug_stat(inode_t node, vfs_stat_t* st);
uint64_t debug_write(inode_t node,
                     void* buffer,
                     uint64_t size,
                     uint64_t offset);
uint64_t debug_isatty(inode_t inode);

static vfs_driver_t debug_driver = {
  NULL,         // open
  NULL,         // close
  NULL,         // read
  debug_write,  // write
  debug_stat,   // stat
  debug_isatty, // isatty
  NULL,         // readdir
  NULL,         // finddir
  NULL,         // cleanup
  NULL,         // create
};

inode_t debug_fs_init()
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "debug");
  node->driver = &debug_driver;
  node->type = FS_CHARDEVICE;

  return node;
}

uint64_t debug_stat(inode_t inode, vfs_stat_t* st)
{
  st->mode |= S_IFCHR;

  return 0;
}

uint64_t debug_write(inode_t inode,
                     void* buffer,
                     uint64_t size,
                     uint64_t offset)
{
  if (offset < size) {
<<<<<<< HEAD
    printf("%s", &((char*)buffer)[offset]);
=======
    printf("%s", (char*)buffer + offset);
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74
    return size - offset;
  }

  return 0;
}

uint64_t debug_isatty(inode_t inode)
{
  return 1;
}
