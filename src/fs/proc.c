#include "proc.h"
#include <core/debug.h>
#include <core/timer.h>
#include <kernel/kmain.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dirent_t* proc_readdir(inode_t inode, uint64_t num);
inode_t proc_finddir(inode_t inode, const char* name);
uint64_t proc_isatty(inode_t node);
uint64_t proc_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
uint64_t proc_stat(inode_t inode, stat_t* st);
uint64_t proc_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset);

vfs_driver_t proc_driver = {
  0,            // open
  0,            // close
  proc_read,    // read
  proc_write,   // write
  proc_stat,    // stat
  proc_isatty,  // isatty
  proc_readdir, // readdir
  proc_finddir, // finddir
};

#define NB_PROC_FILES 5

const char* proc_files[NB_PROC_FILES] = {
  ".", "..", "uptime", "version", "hostname",
};

#define DEFAULT_HOSTNAME    "machine"
#define MAX_HOSTNAME_LENGTH 50

// This is the variable containing the `hostname` value of the system.
// TODO: move this variable somewhere else...
char* hostname = 0;

inode_t proc_fs_init()
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "proc");
  node->driver = &proc_driver;
  node->type = FS_DIRECTORY;

  hostname = malloc(sizeof(char) * (strlen(DEFAULT_HOSTNAME) + 1));
  strcpy(hostname, DEFAULT_HOSTNAME);

  return node;
}

void proc_fs_deinit()
{
  free(hostname);
}

// If the current node is a directory, we need a way of enumerating its
// contents. Readdir should return the n'th child node of a directory or 0
// otherwise. It returns a `dirent_t*`.
dirent_t* proc_readdir(inode_t inode, uint64_t num)
{
  if (vfs_inode_type(inode) != FS_DIRECTORY) {
    return 0;
  }

  if (num < 2 || num >= NB_PROC_FILES) {
    return 0;
  }

  dirent_t* dir = calloc(1, sizeof(dirent_t));
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, proc_files[num]);
  node->driver = inode->driver;
  node->parent = inode;
  node->type = FS_FILE;

  strcpy(dir->name, node->name);
  dir->inode = node;

  DEBUG("returning directory entry=%s", dir->name);

  return dir;
}

inode_t proc_finddir(inode_t inode, const char* name)
{
  for (int idx = 2; idx < NB_PROC_FILES; idx++) {
    if (strcmp(name, proc_files[idx]) == 0) {
      inode_t node = calloc(1, sizeof(vfs_node_t));

      strcpy(node->name, proc_files[idx]);
      node->driver = &proc_driver;
      node->parent = inode;
      node->type = FS_FILE;

      DEBUG("found name=%s type=%ld", node->name, node->type);

      return node;
    }
  }

  return 0;
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

  char buf[256];
  if (strcmp(node->name, "uptime") == 0) {
    // Linux returns two numbers according to the man page (quoted below):
    // http://man7.org/linux/man-pages/man5/proc.5.html.
    //
    // [/proc/uptime] contains two numbers (values in seconds): the uptime of
    // the system (including time spent in suspend) and the amount of time
    // spent in the idle process.
    //
    // We do not have the notion of process yet so there is only one value.
    snprintf(buf, 256, "%ld\n", timer_uptime());
  } else if (strcmp(node->name, "version") == 0) {
    snprintf(buf, 256, "%s %s (%s)\n", KERNEL_NAME, KERNEL_VERSION, GIT_HASH);
  } else if (strcmp(node->name, "hostname") == 0) {
    snprintf(buf, 256, "%s\n", hostname);
  } else {
    return 0;
  }

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

uint64_t proc_stat(inode_t inode, stat_t* st)
{
  memset(st, 0, sizeof(stat_t));
  st->size = 1; // Set a default value to show that the file is not empty.

  if (strcmp(inode->name, "hostname") == 0) {
    st->size = strlen(hostname);
  }

  return 0;
}

uint64_t proc_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset)
{
  if (strcmp(inode->name, "hostname") != 0) {
    return 0;
  }

  if (length > strlen(DEFAULT_HOSTNAME)) {
    if (length > MAX_HOSTNAME_LENGTH) {
      length = MAX_HOSTNAME_LENGTH;
    }

    char* tmp = realloc(hostname, sizeof(char) * (length + 1));

    if (!tmp) {
      DEBUG("%s", "failed to reallocate memory for hostname");
      return 0;
    }

    hostname = tmp;
  }

  strncpy(hostname, ptr, length);

  return strlen(hostname);
}
