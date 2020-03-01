#include "tar.h"
#include <core/debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t get_size(const char* in);
uint64_t tar_read_headers(uint64_t address);
uint64_t tar_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
inode_t tar_finddir(inode_t inode, const char* name);
dirent_t* tar_readdir(inode_t inode, uint64_t num);
uint64_t tar_stat(inode_t node, stat_t* st);
uint64_t tar_isatty(inode_t node);
bool starts_with(const char* s, const char* prefix);
int get_level(const char* path);

tar_header_t* headers[32];

vfs_driver_t tar_driver = {
  0,           // open
  0,           // close
  tar_read,    // read
  0,           // write
  tar_stat,    // stat
  tar_isatty,  // isatty
  tar_readdir, // readdir
  tar_finddir, // finddir
};

inode_t tar_fs_init(uint64_t address)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "tar");
  node->driver = &tar_driver;
  node->type = FS_DIRECTORY;
  node->data = -1;

  tar_read_headers(address);

  return node;
}

uint64_t tar_read_headers(uint64_t address)
{
  uint64_t i;

  for (i = 0;; i++) {
    tar_header_t* header = (tar_header_t*)address;

    if (header->name[0] == '\0') {
      break;
    }

    uint64_t size = get_size(header->size);
    address += ((size / 512) + 1) * 512;

    if (size % 512) {
      address += 512;
    }

    headers[i] = header;
  }

  return i;
}

uint64_t get_size(const char* in)
{
  uint64_t size = 0;
  uint64_t j;
  uint64_t count = 1;

  for (j = 11; j > 0; j--, count *= 8) {
    size += ((in[j - 1] - '0') * count);
  }

  return size;
}

uint64_t tar_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  DEBUG("name=%s type=%d size=%lu offset=%lu",
        node->name,
        vfs_inode_type(node),
        size,
        offset);
  // Empty buffer.
  strcpy(buffer, "");

  if (vfs_inode_type(node) == FS_FILE) {
    tar_header_t* header = headers[node->data];
    uint64_t header_size = get_size(header->size);

    if (size > header_size) {
      size = header_size;
    }

    if (offset > size) {
      offset = size;
    }

    DEBUG("copying %ld bytes (offset=%lu header_size=%ld)",
          size,
          offset,
          header_size);

    memcpy(buffer, (void*)header + 512 + offset, size);
    ((char*)buffer)[size] = '\0';

    return size - offset;
  }

  // TODO(william): add support for other types like symlinks (at least)

  return 0;
}

inode_t tar_finddir(inode_t inode, const char* name)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  char* fullpath;

  if (inode->data >= 0) {
    fullpath = malloc((strlen(headers[inode->data]->name) + strlen(name) + 1) *
                      sizeof(char));
    strcpy(fullpath, headers[inode->data]->name);
    strcat(fullpath, name);
  } else {
    fullpath = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(fullpath, name);
  }

  tar_header_t* header = 0;

  for (uint64_t i = 0; headers[i] != 0; i++) {
    char* header_name = strdup(headers[i]->name);

    // Remove the trailing slash when the current TAR file is a directory.
    if (headers[i]->type == TAR_DIRECTORY) {
      header_name[strlen(header_name) - 1] = '\0';
    }

    DEBUG("comparing fullpath=%s and header_name=%s", fullpath, header_name);

    if (strcmp(fullpath, header_name) == 0) {
      header = headers[i];
      node->data = i;
      free(header_name);
      break;
    }

    free(header_name);
  }

  free(fullpath);

  if (!header) {
    DEBUG("did not find name=%s", name);
    free(node);
    return 0;
  }

  strcpy(node->name, header->name);
  node->driver = &tar_driver;
  node->parent = inode;

  switch (header->type) {
    case TAR_FILE:
      node->type = FS_FILE;
      break;

    case TAR_DIRECTORY:
      node->type = FS_DIRECTORY;
      // Remove the trailing slash when the current TAR file is a directory.
      node->name[strlen(node->name) - 1] = '\0';
      break;
  }

  DEBUG("found name=%s type=%ld", node->name, node->type);

  return node;
}

// If the current node is a directory, we need a way of enumerating its
// contents. Readdir should return the n'th child node of a directory or 0
// otherwise. It returns a `dirent_t*`.
dirent_t* tar_readdir(inode_t inode, uint64_t num)
{
  if (vfs_inode_type(inode) != FS_DIRECTORY) {
    return 0;
  }

  char* name = malloc((strlen(inode->name) + 2) * sizeof(char));

  if (inode->data >= 0) {
    strcpy(name, inode->name);
    // Re-add trailing slash.
    name[strlen(name) + 1] = '\0';
    name[strlen(name)] = '/';
  } else {
    strcpy(name, "");
  }

  int level = get_level(name);

  dirent_t* dir = calloc(1, sizeof(dirent_t));
  inode_t node = calloc(1, sizeof(vfs_node_t));

  uint64_t i = 0;
  uint64_t j = -1;
  bool found = false;

  while (headers[i] != 0) {
    int header_level = get_level(headers[i]->name);

    if (headers[i]->type == TAR_DIRECTORY) {
      header_level--;
    }

    DEBUG("comparing header_name=%s and name=%s", headers[i]->name, name);

    if (starts_with(headers[i]->name, name) && level == header_level) {
      j++;
    }

    if ((j + 2) == num) {
      found = true;

      switch (headers[i]->type) {
        case TAR_FILE:
          node->type = FS_FILE;
          break;

        case TAR_DIRECTORY:
          node->type = FS_DIRECTORY;
          break;
      }

      // Only copy the name of the inode without `name` (which is likely
      // the path to this inode).
      strcpy(node->name, headers[i]->name + strlen(name));
      node->data = i;
      break;
    }

    i++;
  }

  free(name);

  if (!found) {
    free(dir);
    free(node);

    return 0;
  }

  node->driver = inode->driver;
  node->parent = inode;

  strcpy(dir->name, node->name);
  dir->inode = node;

  DEBUG("returning directory entry=%s", dir->name);

  return dir;
}

uint64_t tar_stat(inode_t inode, stat_t* st)
{
  memset(st, 0, sizeof(stat_t));

  st->size = 0;

  if (vfs_inode_type(inode) == FS_FILE) {
    st->size = get_size(headers[inode->data]->size);
  }

  return 0;
}

uint64_t tar_isatty(inode_t node)
{
  return 0;
}

bool starts_with(const char* s, const char* prefix)
{
  return strncmp(s, prefix, strlen(prefix)) == 0 ? true : false;
}

int get_level(const char* path)
{
  int level = 0;

  for (int i = 0; i < strlen(path); i++) {
    if (path[i] == '/') {
      level++;
    }
  }

  return level;
}
