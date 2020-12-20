#include "vfs.h"
#include "logging.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

inode_t vfs_find_root(char** path);
inode_t vfs_namei_mount(const char* path, inode_t root);

static vfs_node_t* vfs_root = NULL;

void vfs_init()
{
  vfs_root = calloc(1, sizeof(vfs_node_t));
  strcpy(vfs_root->name, "/");
  vfs_root->parent = vfs_root;
  vfs_root->type = FS_DIRECTORY | FS_MOUNTPOINT;
  vfs_root->n_children = 0;
  vfs_root->children = (inode_t*)calloc(0, sizeof(inode_t));
}

uint64_t vfs_open(inode_t inode, uint64_t mode)
{
  if (inode->driver && inode->driver->open) {
    return inode->driver->open(inode, mode);
  }

  return 0;
}

uint64_t vfs_close(inode_t inode)
{
  if (inode->driver && inode->driver->close) {
    return inode->driver->close(inode);
  }

  return 0;
}

inode_t vfs_create(inode_t parent, const char* name, uint64_t flags)
{
  if (parent->driver && parent->driver->create) {
    return parent->driver->create(parent, name, flags);
  }

  return NULL;
}

uint64_t vfs_read(inode_t inode, void* ptr, uint64_t length, uint64_t offset)
{
  if (inode->driver && inode->driver->read) {
    return inode->driver->read(inode, ptr, length, offset);
  }

  return 0;
}

uint64_t vfs_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset)
{
  if (inode->driver && inode->driver->write) {
    return inode->driver->write(inode, ptr, length, offset);
  }

  return 0;
}

uint64_t vfs_stat(inode_t inode, vfs_stat_t* stat)
{
  memset(stat, 0, sizeof(vfs_stat_t));

  switch (inode->type) {
    case FS_FILE:
      stat->mode = S_IFREG;
      break;
    case FS_DIRECTORY:
      stat->mode = S_IFDIR;
      break;
    case FS_CHARDEVICE:
      stat->mode = S_IFCHR;
      break;
  }

  if (inode->driver && inode->driver->stat) {
    uint64_t ret = inode->driver->stat(inode, stat);
    stat->size += inode->n_children;

    return ret;
  }

  return 0;
}

uint64_t vfs_isatty(inode_t inode)
{
  if (inode->driver && inode->driver->isatty) {
    return inode->driver->isatty(inode);
  }

  return 0;
}

dirent_t* vfs_readdir(inode_t inode, uint64_t num)
{
  if (inode == NULL) {
    FS_DEBUG("%s", "inode is NULL");
    return NULL;
  }

  if (vfs_type(inode) != FS_DIRECTORY) {
    FS_DEBUG("%s", "inode is not a directory");
    return NULL;
  }

  dirent_t* de = NULL;

  if (num == 0) {
    de = calloc(1, sizeof(dirent_t));
    de->inode = inode;
    strcpy(de->name, ".");
    return de;
  }

  if (num == 1) {
    de = calloc(1, sizeof(dirent_t));
    de->inode = inode->parent;
    strcpy(de->name, "..");
    return de;
  }

  bool is_mountpoint = (inode->type & FS_MOUNTPOINT) == FS_MOUNTPOINT;

  if (is_mountpoint && inode->n_children > (num - 2)) {
    de = calloc(1, sizeof(dirent_t));
    de->inode = inode->children[num - 2];
    strcpy(de->name, inode->children[num - 2]->name);
    return de;
  }

  if (inode->driver && inode->driver->readdir) {
    if (is_mountpoint) {
      num -= inode->n_children;
    }

    de = inode->driver->readdir(inode, num);
  }

  return de;
}

inode_t vfs_finddir(inode_t inode, const char* name)
{
  if (vfs_type(inode) == FS_DIRECTORY) {
    if (!strcmp(name, ".")) {
      return inode;
    }

    if (!strcmp(name, "..")) {
      return inode->parent;
    }

    if (inode->driver && inode->driver->finddir) {
      return inode->driver->finddir(inode, name);
    }
  }

  return NULL;
}

inode_t vfs_find_root(char** path)
{
  inode_t current = vfs_root;
  inode_t mount = current;

  char* name = NULL;
  while ((name = strsep(path, "/")) != 0) {
    uint64_t i = 0;
    for (; i < current->n_children; i++) {
      if (strcmp(current->children[i]->name, name) == 0) {
        mount = current->children[i];
        break;
      }
    }

    if (i == current->n_children) {
      if (*path) {
        *path = *path - 1;
        *path[0] = '/';
      }

      *path = name;
      break;
    }
  }

  FS_DEBUG("found root node: %s", mount->name);

  return mount;
}

inode_t vfs_namei_mount(const char* path, inode_t root)
{
  char* npath = strdup(path);
  char* pth = &npath[1];

  inode_t current = vfs_find_root(&pth);

  if (strcmp(path, "/") != 0) {
    char* name = NULL;
    while (current && (name = strsep(&pth, "/")) != 0) {
      FS_DEBUG("current node is: %s", current->name);

      inode_t next = vfs_finddir(current, name);

      if (next == NULL) {
        // Try to find a mounted FS, if any.
        for (uint64_t i = 0; i < current->n_children; i++) {
          if (strcmp(current->children[i]->name, name) == 0) {
            next = current->children[i];
            break;
          }
        }
      }

      if (root) {
        if (!next) {
          if (pth != NULL) {
            FS_DEBUG("cannot create a node because pth=%s", pth);
            free(npath);
            return NULL;
          }

          next = calloc(1, sizeof(vfs_node_t));
          next->n_children = 0;
          next->children = (inode_t*)calloc(0, sizeof(inode_t));
          next->type = FS_DIRECTORY;
          strcpy(next->name, name);

          FS_DEBUG("created node: %s", next->name);
        }

        next->parent = current;

        FS_DEBUG("adding child #%lu to current node: %s",
                 current->n_children + 1,
                 current->name);
        current->children = (inode_t*)realloc(
          current->children, (current->n_children + 1) * sizeof(inode_t));
        current->children[current->n_children++] = next;
      }

      if (next == NULL) {
        FS_DEBUG("%s", "next is NULL, returning NULL");
        free(npath);
        return NULL;
      }

      current = next;
    }
  }

  free(npath);

  if (root != NULL && vfs_type(current) == FS_DIRECTORY) {
    FS_DEBUG(
      "attempting to mount root_fs=%s (type=0x%x) at node=%s (parent=%s)",
      root->name,
      vfs_type(root),
      current->name,
      current->parent->name);

    current->type = root->type | FS_MOUNTPOINT;
    current->driver = root->driver;
    current->data = root->data;

    free(root);
  }

  FS_DEBUG("returning current node: %s", current->name);

  return current;
}

inode_t vfs_mount(const char* path, inode_t root)
{
  if (vfs_root == NULL) {
    DEBUG("%s", "vfs_root is NULL. vfs_init() not called?");
    return NULL;
  }

  if (path == NULL || path[0] != '/') {
    DEBUG("path must be absolute: %s", path);
    return NULL;
  }

  FS_DEBUG("trying to mount fs=%s at path=%s", root->name, path);
  return vfs_namei_mount(path, root);
}

inode_t vfs_namei(const char* path)
{
  FS_DEBUG("trying to find node for path=%s", path);
  return vfs_namei_mount(path, NULL);
}

int vfs_umount(const char* path)
{
  FS_DEBUG("trying to unmount path=%s", path);
  inode_t node = vfs_namei(path);

  if (node == NULL) {
    FS_DEBUG("no node found for path=%s", path);
    return FS_ERR_NOTFOUND;
  }

  if ((node->type & FS_MOUNTPOINT) != FS_MOUNTPOINT) {
    FS_DEBUG("%s", "node is not mountpoint");
    return FS_ERR_NOTAMOUNTPOINT;
  }

  if (node->n_children > 0) {
    FS_DEBUG("%s", "node has children");
    return FS_ERR_HASCHILDREN;
  }

  inode_t parent = node->parent;
  inode_t* new_children =
    (inode_t*)malloc(sizeof(inode_t) * parent->n_children - 1);

  for (uint64_t i = 0, j = 0; i < parent->n_children; i++) {
    if (parent->children[i] != node) {
      new_children[j++] = parent->children[i];
    }
  }

  free(parent->children);
  parent->children = new_children;
  parent->n_children--;

  node->parent = NULL;
  vfs_free(node);
  node = NULL;

  return 0;
}

int vfs_free(inode_t inode)
{
  if ((inode->type & FS_MOUNTPOINT) == FS_MOUNTPOINT) {
    inode->parent = NULL;
  }

  if (inode->parent != NULL) {
    DEBUG("%s", "cannot free a node with a parent");
    return FS_ERR_HASPARENT;
  }

  FS_DEBUG("free'ing node: %s", inode->name);

  for (uint64_t i = 0; i < inode->n_children; i++) {
    inode->children[i]->parent = NULL;
    vfs_free(inode->children[i]);
  }

  free(inode->children);

  if (inode->driver && inode->driver->cleanup) {
    inode->driver->cleanup(inode);
  }

  free(inode);

  return 0;
}

int vfs_type(inode_t inode)
{
  if (inode == NULL) {
    return 0;
  }

  return inode->type & FS_MASK;
}

void vfs_debug(inode_t node, int depth)
{
  if (depth == 0) {
    printf(".\n");
  }

  for (int i = 0; i < (depth * 4); i++) {
    printf(" ");
  }
  printf("└── %s\n", node->name);

  depth++;

  for (int i = 0; i < node->n_children; i++) {
    vfs_debug(node->children[i], depth);
  }
}
