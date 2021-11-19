#include <fs/tar.h>

#include <arvern/utils.h>
#include <fs/logging.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t oct_to_int(const char* in);
uint64_t tar_read_headers(uint64_t address);
uint64_t tar_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
inode_t tar_finddir(inode_t inode, const char* name);
dirent_t* tar_readdir(inode_t inode, uint64_t num);
uint64_t tar_stat(inode_t inode, vfs_stat_t* st);
uint64_t tar_isatty(inode_t inode);
void tar_cleanup(inode_t inode);
bool starts_with(const char* s, const char* prefix);
int get_level(const char* path);

static tar_header_t* headers[32];
static uint64_t nb_entries = 0;
static inode_t* nodes = NULL;

static vfs_driver_t tar_driver = {
  NULL,        // open
  NULL,        // close
  tar_read,    // read
  NULL,        // write
  tar_stat,    // stat
  tar_isatty,  // isatty
  tar_readdir, // readdir
  tar_finddir, // finddir
  tar_cleanup, // cleanup
  NULL,        // create
};

inode_t tar_fs_create(uint64_t address)
{
  inode_t node = vfs_make_directory("tar");
  node->driver = &tar_driver;
  node->data = -1;

  nb_entries = tar_read_headers(address);
  FS_DEBUG("read %lu entries", nb_entries);
  nodes = calloc(1, nb_entries * sizeof(inode_t));

  return node;
}

uint64_t tar_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  FS_DEBUG("name=%s type=%d size=%lu offset=%lu",
           node->name,
           vfs_type(node),
           size,
           offset);
  // Empty buffer.
  strcpy(buffer, "");

  if (vfs_type(node) == FS_FILE) {
    tar_header_t* header = headers[node->data];
    uint64_t header_size = oct_to_int(header->size);

    if (offset > header_size) {
      return 0;
    }

    if (size > header_size) {
      size = header_size;
    }

    if (offset + size > header_size) {
      size = header_size - offset;
    }

    FS_DEBUG("copying %ld bytes (offset=%lu header_size=%ld) to buffer=%p",
             size,
             offset,
             header_size,
             buffer);

    memcpy(buffer, (char*)header + 512 + offset, size);

    return size;
  }

  // TODO: add support for other types like symlinks (at least)

  return 0;
}

inode_t tar_finddir(inode_t inode, const char* name)
{
  FS_DEBUG("trying to find: %s", name);

  inode_t node = calloc(1, sizeof(vfs_node_t));

  char* fullpath = NULL;

  if (inode->data >= 0) {
    fullpath = malloc((strlen(headers[inode->data]->name) + strlen(name) + 1) *
                      sizeof(char));
    strcpy(fullpath, headers[inode->data]->name);
    strcat(fullpath, name);
  } else {
    fullpath = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(fullpath, name);
  }

  tar_header_t* header = NULL;

  for (uint64_t i = 0; headers[i] != 0; i++) {
    char* header_name = strdup(headers[i]->name);

    // Remove the trailing slash when the current TAR file is a directory.
    if (headers[i]->type == TAR_DIRECTORY) {
      header_name[strlen(header_name) - 1] = '\0';
    }

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
    FS_DEBUG("did not find name=%s", name);
    free(node);
    return NULL;
  }

  if (nodes[node->data] == NULL) {
    strcpy(node->name, header->name);
    node->driver = &tar_driver;
    node->parent = inode;

    FS_DEBUG("adding node #%" PRIi64 " to the list of nodes", node->data);
    nodes[node->data] = node;

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
  } else {
    inode_t tmp = nodes[node->data];
    free(node);
    node = tmp;
  }

  FS_DEBUG("found node name=%s type=%lu",
           nodes[node->data]->name,
           nodes[node->data]->type);

  return nodes[node->data];
}

// If the current node is a directory, we need a way of enumerating its
// contents. Readdir should return the n'th child node of a directory or 0
// otherwise. It returns a `dirent_t*`.
dirent_t* tar_readdir(inode_t inode, uint64_t num)
{
  if (vfs_type(inode) != FS_DIRECTORY) {
    return NULL;
  }

  num -= 2;

  dirent_t* dir = calloc(1, sizeof(dirent_t));

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

  inode_t node = calloc(1, sizeof(vfs_node_t));

  uint64_t i = 0;
  uint64_t j = -1;
  bool found = false;

  while (headers[i] != 0) {
    int header_level = get_level(headers[i]->name);

    if (headers[i]->type == TAR_DIRECTORY) {
      header_level--;
    }

    if (starts_with(headers[i]->name, name) && level == header_level) {
      j++;
    }

    if (j == num) {
      found = true;

      strcpy(node->name, headers[i]->name);
      node->driver = inode->driver;
      node->parent = inode;
      node->data = i;

      switch (headers[i]->type) {
        case TAR_FILE:
          node->type = FS_FILE;
          break;

        case TAR_DIRECTORY:
          node->type = FS_DIRECTORY;
          // Remove the trailing slash when the current TAR file is a
          // directory.
          node->name[strlen(node->name) - 1] = '\0';
          break;
      }

      if (nodes[i] != NULL) {
        free(node);
      } else {
        // Add to "cache".
        nodes[i] = node;
      }

      break;
    }

    i++;
  }

  if (!found) {
    free(name);
    free(dir);

    return 0;
  }

  // Only copy the name of the inode without `name` (which is
  // likely the path to this inode).
  strcpy(dir->name, nodes[i]->name + strlen(name));
  dir->inode = nodes[i];

  FS_DEBUG("returning directory entry=%s", dir->name);
  free(name);

  return dir;
}

uint64_t tar_stat(inode_t inode, vfs_stat_t* st)
{
  // TODO: add support for directories

  if (vfs_type(inode) == FS_FILE) {
    st->size = oct_to_int(headers[inode->data]->size);
  }

  return 0;
}

uint64_t tar_isatty(inode_t inode)
{
  UNUSED(inode);

  return 0;
}

void tar_cleanup(inode_t inode)
{
  FS_DEBUG("cleaning up %s", inode->name);
  UNUSED(inode);

  for (uint64_t i = 0; i < nb_entries; i++) {
    if (nodes[i] != NULL) {
      nodes[i]->parent = NULL;
      free(nodes[i]);
    }
  }

  free(nodes);
}

bool starts_with(const char* s, const char* prefix)
{
  return strncmp(s, prefix, strlen(prefix)) == 0 ? true : false;
}

int get_level(const char* path)
{
  int level = 0;

  for (size_t i = 0; i < strlen(path); i++) {
    if (path[i] == '/') {
      level++;
    }
  }

  return level;
}

uint64_t oct_to_int(const char* in)
{
  uint64_t size = 0;

  for (uint64_t j = 11, count = 1; j > 0; j--, count *= 8) {
    size += ((in[j - 1] - '0') * count);
  }

  return size;
}

uint64_t tar_read_headers(uint64_t address)
{
  uint64_t i = 0;

  for (i = 0;; i++) {
    tar_header_t* header = (tar_header_t*)address;

    if (header->name[0] == '\0') {
      FS_DEBUG("%s", "EOF");
      break;
    }

    uint64_t size = oct_to_int(header->size);
    address += ((size / 512) + 1) * 512;

    FS_DEBUG("found '%s' with size=%lu", header->name, size);

    if (size % 512) {
      address += 512;
    }

    headers[i] = header;
  }

  return i;
}
