#include "proc.h"
#include <core/debug.h>
#include <core/timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dirent_t* proc_readdir(inode_t inode, uint64_t num);
inode_t proc_finddir(inode_t inode, const char* name);
uint64_t proc_isatty(inode_t node);
uint64_t proc_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
uint64_t proc_stat(inode_t node, stat_t* st);

vfs_driver_t proc_driver = {
  0,            // open
  0,            // close
  proc_read,    // read
  0,            // write
  0,            // link
  0,            // unlink
  proc_stat,    // stat
  proc_isatty,  // isatty
  0,            // mkdir
  proc_readdir, // readdir
  proc_finddir, // finddir
};

inode_t proc_fs_init()
{
  inode_t node = malloc(sizeof(vfs_node_t));

  strcpy(node->name, "proc");
  node->driver = &proc_driver;
  node->type = FS_DIRECTORY;

  return node;
}

// If the current node is a directory, we need a way of enumerating it's
// contents. Readdir should return the n'th child node of a directory or 0
// otherwise. It returns a `dirent_t*`.
dirent_t* proc_readdir(inode_t inode, uint64_t num)
{
  if (vfs_inode_type(inode) != FS_DIRECTORY) {
    return 0;
  }

  if (num < 2) {
    return 0;
  }

  dirent_t* dir = malloc(sizeof(dirent_t));
  inode_t node = malloc(sizeof(vfs_node_t));

  node->driver = inode->driver;
  node->parent = inode;
  node->type = FS_FILE;

  switch (num) {
    case 2:
      strcpy(node->name, "uptime");
      break;
    default:
      // That is because we do not support other files yet.
      return 0;
  }

  strcpy(dir->name, node->name);
  dir->inode = node;

  DEBUG("returning directory entry=%s", dir->name);

  return dir;
}

inode_t proc_finddir(inode_t inode, const char* name)
{
  if (strncmp(name, "uptime", 6) != 0) {
    return 0;
  }

  inode_t node = malloc(sizeof(vfs_node_t));

  strcpy(node->name, "uptime");
  node->driver = &proc_driver;
  node->parent = inode;
  node->type = FS_FILE;

  DEBUG("found name=%s type=%ld", node->name, node->type);

  return node;
}

uint64_t proc_isatty(inode_t node)
{
  return 0;
}

uint64_t proc_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  DEBUG("name=%s type=%d size=%lu offset=%lu",
        node->name,
        vfs_inode_type(node),
        size,
        offset);
  // Empty buffer.
  strcpy(buffer, "");

  if (vfs_inode_type(node) != FS_FILE) {
    return 0;
  }

  if (strncmp(node->name, "uptime", 6) == 0) {
    char buf[40];
    // Linux returns two numbers according to the man page (quoted below):
    // http://man7.org/linux/man-pages/man5/proc.5.html.
    //
    // [/proc/uptime] contains two numbers (values in seconds): the uptime of
    // the system (including time spent in suspend) and the amount of time
    // spent in the idle process.
    //
    // We do not have the notion of process yet so there is only one value.
    snprintf(buf, 40, "%ld\n", timer_uptime());
    uint8_t len = strlen(buf);

    if (size > len) {
      size = len;
    }

    if (offset > size) {
      offset = size;
    }

    memcpy(buffer, (void*)buf + offset, size);
    ((char*)buffer)[size] = '\0';

    return size - offset;
  }

  return 0;
}

uint64_t proc_stat(inode_t node, stat_t* st)
{
  memset(st, 0, sizeof(stat_t));
  st->size = 0;
  return 0;
}
