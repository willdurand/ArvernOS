#include "sock.h"
#include "logging.h"
#include <core/utils.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

uint64_t sock_open(inode_t inode, uint64_t flags);
uint64_t sock_close(inode_t inode);
inode_t sock_create(inode_t parent, const char* name, uint64_t flags);
uint64_t sock_read(inode_t inode, void* buffer, uint64_t size, uint64_t offset);
uint64_t sock_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
uint64_t sock_stat(inode_t inode, vfs_stat_t* st);
dirent_t* sock_readdir(inode_t inode, uint64_t num);

static sock_entry_t entries[MAX_ENTRIES] = { 0 };
static uint64_t nb_used_entries = 0;

static vfs_driver_t sock_driver = {
  NULL,         // open
  sock_close,   // close
  sock_read,    // read
  sock_write,   // write
  sock_stat,    // stat
  NULL,         // isatty
  sock_readdir, // readdir
  NULL,         // finddir
  NULL,         // cleanup
  sock_create   // create
};

bool sock_fs_init()
{
  return (bool)vfs_mount(FS_SOCK_MOUNTPOINT, sock_fs_create());
}

inode_t sock_fs_create()
{
  inode_t node = vfs_make_directory("sock");
  node->driver = &sock_driver;

  return node;
}

inode_t sock_create(inode_t parent, const char* name, uint64_t flags)
{
  UNUSED(name);
  UNUSED(flags);

  for (uint64_t i = 0; i < MAX_ENTRIES; i++) {
    if (!entries[i].inode) {
      inode_t inode = calloc(1, sizeof(vfs_node_t));
      sprintf(inode->name, "%lu.sock", i + 1);
      inode->driver = &sock_driver;
      inode->type = FS_FILE;
      inode->parent = parent;
      inode->data = i;

      entries[i].inode = inode;
      nb_used_entries++;

      FS_DEBUG("created entry #%lu", i);
      return inode;
    }
  }

  FS_DEBUG("%s", "no more available entries");
  return NULL;
}

uint64_t sock_close(inode_t inode)
{
  uint64_t idx = inode->data;
  FS_DEBUG("attempting to close entry #%lu", idx);

  if (entries[idx].inode != NULL) {
    free(inode);
    entries[idx].inode = NULL;
    nb_used_entries--;
  }

  return 0;
}

// If the current node is a directory, we need a way of enumerating its
// contents. Readdir should return the n'th child node of a directory or NULL
// otherwise. It returns a `dirent_t*`.
dirent_t* sock_readdir(inode_t inode, uint64_t num)
{
  if (vfs_type(inode) != FS_DIRECTORY) {
    return NULL;
  }

  num -= 2;
  dirent_t* de = NULL;

  if (entries[num].inode != NULL) {
    de = calloc(1, sizeof(dirent_t));
    de->inode = entries[num].inode;
    strcpy(de->name, entries[num].inode->name);

    FS_DEBUG("returning directory entry=%s", de->name);
  }

  return de;
}

uint64_t sock_read(inode_t inode, void* buffer, uint64_t size, uint64_t offset)
{
  UNUSED(offset);

  sock_entry_t* entry = &entries[inode->data];

  FS_DEBUG("found entry for inode: %s len=%d read=%d write=%d",
           inode->name,
           entry->len,
           entry->read,
           entry->write);

  for (size_t i = 0; i < size; i++) {
    if (entry->len == 0) {
      FS_DEBUG("buffer is empty for: %s", inode->name);
      return i;
    }

    ((uint8_t*)buffer)[i] = entry->buffer[entry->read++];
    entry->len--;

    if (entry->read == ENTRY_BUF_LEN) {
      entry->read = 0;
    }
  }

  FS_DEBUG("unbufferized %lu bytes", size);
  return size;
}

uint64_t sock_stat(inode_t inode, vfs_stat_t* st)
{
  if (vfs_type(inode) == FS_DIRECTORY) {
    st->size = nb_used_entries;
    return 0;
  }

  sock_entry_t entry = entries[inode->data];

  if (entry.inode != NULL) {
    st->size = entry.len;
    st->mode |= S_IFSOCK;
  }

  return 0;
}

uint64_t sock_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset)
{
  UNUSED(offset);

  sock_entry_t* entry = &entries[inode->data];

  if (!entry->inode) {
    FS_DEBUG("missing entry for inode: %s", inode->name);
    return 0;
  }

  uint64_t i;
  for (i = 0; i < length; i++) {
    if (entry->len == ENTRY_BUF_LEN) {
      FS_DEBUG("buffer is full for inode: %s", inode->name);
      break;
    }

    entry->buffer[entry->write++] = ((uint8_t*)ptr)[i];
    entry->len++;

    if (entry->write == ENTRY_BUF_LEN) {
      entry->write = 0;
    }
  }

  FS_DEBUG("added %lu bytes to inode: %s len=%d read=%d write=%d",
           i,
           inode->name,
           entry->len,
           entry->read,
           entry->write);

  return i;
}
