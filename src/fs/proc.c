#include "proc.h"
#include "logging.h"
#include <drivers/timer.h>
#include <kernel/kmain.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dirent_t* proc_readdir(inode_t inode, uint64_t num);
inode_t proc_finddir(inode_t inode, const char* name);
uint64_t proc_isatty(inode_t node);
uint64_t proc_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
uint64_t proc_stat(inode_t inode, stat_t* st);
uint64_t proc_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
void proc_cleanup(inode_t inode);

static vfs_driver_t proc_driver = {
  NULL,         // open
  NULL,         // close
  proc_read,    // read
  proc_write,   // write
  proc_stat,    // stat
  proc_isatty,  // isatty
  proc_readdir, // readdir
  proc_finddir, // finddir
  proc_cleanup  // cleanup
};

#define NB_PROC_FILES 4

static const char* proc_files[NB_PROC_FILES] = {
  "hostname",
  "meminfo",
  "uptime",
  "version",
};
static inode_t nodes[NB_PROC_FILES];

#define DEFAULT_HOSTNAME    "machine"
#define MAX_HOSTNAME_LENGTH 50

// This is the variable containing the `hostname` value of the system.
// TODO: move this variable somewhere else...
static char* hostname = NULL;

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

// If the current node is a directory, we need a way of enumerating its
// contents. Readdir should return the n'th child node of a directory or 0
// otherwise. It returns a `dirent_t*`.
dirent_t* proc_readdir(inode_t inode, uint64_t num)
{
  if (vfs_inode_type(inode) != FS_DIRECTORY) {
    return NULL;
  }

  num -= 2;

  if (num >= NB_PROC_FILES) {
    return NULL;
  }

  dirent_t* dir = calloc(1, sizeof(dirent_t));

  if (nodes[num] == NULL) {
    inode_t node = calloc(1, sizeof(vfs_node_t));

    strcpy(node->name, proc_files[num]);
    node->driver = inode->driver;
    node->parent = inode;
    node->type = FS_FILE;

    nodes[num] = node;
  }

  strcpy(dir->name, nodes[num]->name);
  dir->inode = nodes[num];

  FS_DEBUG("returning directory entry=%s", dir->name);

  return dir;
}

inode_t proc_finddir(inode_t inode, const char* name)
{
  for (int idx = 0; idx < NB_PROC_FILES; idx++) {
    if (strcmp(name, proc_files[idx]) == 0) {
      if (nodes[idx] == NULL) {
        inode_t node = calloc(1, sizeof(vfs_node_t));

        strcpy(node->name, proc_files[idx]);
        node->driver = &proc_driver;
        node->parent = inode;
        node->type = FS_FILE;

        nodes[idx] = node;
      }

      FS_DEBUG("found name=%s type=%ld", nodes[idx]->name, nodes[idx]->type);

      return nodes[idx];
    }
  }

  return NULL;
}

uint64_t proc_isatty(inode_t node)
{
  return 0;
}

uint64_t proc_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  FS_DEBUG("name=%s type=%d size=%lu offset=%lu",
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
    snprintf(buf, 256, "%lu\n", timer_uptime());
  } else if (strcmp(node->name, "version") == 0) {
    snprintf(buf, 256, "%s %s (%s)\n", KERNEL_NAME, KERNEL_VERSION, GIT_HASH);
  } else if (strcmp(node->name, "hostname") == 0) {
    snprintf(buf, 256, "%s\n", hostname);
  } else if (strcmp(node->name, "meminfo") == 0) {
    uint64_t used_frames = frame_get_used_count();
    uint64_t max_frames = frame_get_max_count();
    uint64_t used_heap = alloc_get_used_count();
    uint64_t max_heap = alloc_get_max_count();

    snprintf(buf,
             256,
             "frames: %6lu/%lu\nheap  : %6lu/%lu KiB [%lu/%lu]\n",
             used_frames,
             max_frames,
             (used_heap * PAGE_SIZE) / 1024,
             (max_heap * PAGE_SIZE) / 1024,
             used_heap,
             max_heap);
  } else {
    return 0;
  }

  uint8_t len = strlen(buf);

  if (offset > len) {
    return 0;
  }

  if (size > len) {
    size = len;
  }

  if (offset + size > len) {
    size = len - offset;
  }

  memcpy(buffer, (char*)buf + offset, size);
  ((char*)buffer)[size] = '\0';

  return size;
}

uint64_t proc_stat(inode_t inode, stat_t* st)
{
  if ((inode->type & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
    st->size = NB_PROC_FILES;
  }

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

  return proc_update_hostname((char*)ptr, length);
}

uint64_t proc_update_hostname(char* new_hostname, uint64_t length)
{
  if (length > MAX_HOSTNAME_LENGTH) {
    length = MAX_HOSTNAME_LENGTH;
  }

  char* tmp = realloc(hostname, sizeof(char) * (length + 1));

  if (!tmp) {
    FS_DEBUG("%s", "failed to reallocate memory for hostname");
    return 0;
  }

  hostname = tmp;

  strncpy(hostname, new_hostname, length + 1);

  return strlen(hostname);
}

void proc_cleanup(inode_t inode)
{
  FS_DEBUG("cleaning up %s", inode->name);

  for (uint64_t i = 0; i < NB_PROC_FILES; i++) {
    if (nodes[i] != NULL) {
      nodes[i]->parent = NULL;
      free(nodes[i]);
    }
  }

  free(hostname);
}
