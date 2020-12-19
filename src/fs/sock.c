#include "sock.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t sock_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
uint64_t sock_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
uint64_t sock_stat(inode_t inode, stat_t* st);
uint64_t sock_isatty(inode_t node);
dirent_t* sock_readdir(inode_t inode, uint64_t num);
inode_t sock_finddir(inode_t inode, const char* name);

static vfs_driver_t sock_driver = {
  NULL,         // open
  NULL,         // close
  sock_read,    // read
  sock_write,   // write
  sock_stat,    // stat
  sock_isatty,  // isatty
  sock_readdir, // readdir
  sock_finddir, // finddir
  NULL          // cleanup
};

inode_t sock_fs_init()
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "sock");
  node->driver = &sock_driver;
  node->type = FS_DIRECTORY;

  return node;
}

// If the current node is a directory, we need a way of enumerating its
// contents. Readdir should return the n'th child node of a directory or NULL
// otherwise. It returns a `dirent_t*`.
dirent_t* sock_readdir(inode_t inode, uint64_t num)
{
  if (inode->type != FS_DIRECTORY) {
    return NULL;
  }

  return NULL;
}

inode_t sock_finddir(inode_t inode, const char* name)
{
  return NULL;
}

uint64_t sock_isatty(inode_t node)
{
  return 0;
}

uint64_t sock_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  return 0;
}

uint64_t sock_stat(inode_t inode, stat_t* st)
{
  return 0;
}

uint64_t sock_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset)
{
  return 0;
}
