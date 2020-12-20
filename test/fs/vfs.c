#include <fs/vfs.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

#define NB_NODES 1

uint64_t test_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
dirent_t* test_readdir(inode_t inode, uint64_t num);

inode_t nodes_for_testfs[NB_NODES];

static vfs_driver_t test_driver = {
  NULL,         // open
  NULL,         // close
  test_read,    // read
  NULL,         // write
  NULL,         // stat
  NULL,         // isatty
  test_readdir, // readdir
  NULL          // finddir
};

inode_t test_fs_init(const char* name)
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, name);
  node->driver = &test_driver;
  node->type = FS_DIRECTORY;

  if (strcmp(name, "test-2") == 0) {
    for (uint8_t i = 0; i < NB_NODES; i++) {
      nodes_for_testfs[i] = calloc(1, sizeof(vfs_node_t));
      sprintf(nodes_for_testfs[i]->name, "%s-file-%d", node->name, i + 1);
      nodes_for_testfs[i]->parent = node;
      nodes_for_testfs[i]->driver = &test_driver;
    }
  }

  return node;
}

uint64_t test_read(inode_t node, void* buffer, uint64_t size, uint64_t offset)
{
  strcpy(buffer, node->name + offset);

  return strlen(node->name) - offset;
}

dirent_t* test_readdir(inode_t inode, uint64_t num)
{
  if (num < 2) {
    return NULL;
  }

  num -= 2;

  if (num >= NB_NODES) {
    return NULL;
  }

  dirent_t* de = calloc(1, sizeof(dirent_t));
  strcpy(de->name, nodes_for_testfs[num]->name);
  de->inode = nodes_for_testfs[num];

  return de;
}

inode_t chardevfs_init()
{
  inode_t node = calloc(1, sizeof(vfs_node_t));

  strcpy(node->name, "chardevfs");
  node->type = FS_CHARDEVICE;

  return node;
}

int main()
{
  char buf[256];
  int retval;

  vfs_init();

  describe("vfs_namei()");
  inode_t root = vfs_namei("/");
  assert(root != NULL, "creates a valid inode for /");
  assert(strcmp(root->name, "/") == 0, "has a valid name");
  assert(root->parent == root, "has a parent set to itself");
  end_describe();

  describe("vfs_mount()");
  inode_t m = vfs_mount("/test1", NULL);
  assert(m == NULL, "returns null when root fs is invalid");

  inode_t test1 = vfs_mount("/test1", test_fs_init("test-1"));
  assert(test1 != NULL, "returns the test fs mountpoint");
  assert(strcmp(test1->name, "test1") == 0, "sets the correct name");
  assert(test1->parent == root, "has root as parent");
  assert(vfs_type(test1) == FS_DIRECTORY, "sets the correct type");

  inode_t test2 = vfs_mount("/test2", test_fs_init("test-2"));
  assert(strcmp(test2->name, "test2") == 0, "sets the correct name");
  assert(vfs_type(test2) == FS_DIRECTORY, "sets the correct type");
  assert(test2->parent == root, "has root as parent");

  inode_t test3 = vfs_mount("/test2/test3", test_fs_init("test-3"));
  assert(strcmp(test3->name, "test3") == 0, "sets the correct name");
  assert(vfs_type(test3) == FS_DIRECTORY, "sets the correct type");
  assert(test3->parent == test2, "has test2 as parent");

  inode_t chardev = vfs_mount("/chardev", chardevfs_init());
  assert(strcmp(chardev->name, "chardev") == 0, "sets the correct name");
  assert(vfs_type(chardev) == FS_CHARDEVICE, "sets the correct type");
  end_describe();

  describe("vfs_read()");
  retval = vfs_read(test1, buf, sizeof(buf), 0);
  assert(strcmp(buf, "test1") == 0, "calls the node's driver to read");
  assert(retval == 5, "returns the number of chars read");
  retval = vfs_read(test2, buf, sizeof(buf), 0);
  assert(strcmp(buf, "test2") == 0, "calls the node's driver to read");
  assert(retval == 5, "returns the number of chars read");
  retval = vfs_read(test3, buf, sizeof(buf), 0);
  assert(strcmp(buf, "test3") == 0, "calls the node's driver to read");
  assert(retval == 5, "returns the number of chars read");
  retval = vfs_read(test3, buf, sizeof(buf), 4);
  assert(strcmp(buf, "3") == 0, "reads with an offset");
  assert(retval == 1, "returns the number of chars read");
  end_describe();

  describe("vfs_readdir()");
  uint64_t num = 0;
  while (num < 10) {
    dirent_t* de = vfs_readdir(test2, num++);

    if (de == NULL) {
      break;
    }

    printf("%s\n", de->name);
    free(de);
  }

  assert(vfs_readdir(chardev, 0) == NULL, "returns NULL for non-directories");
  end_describe();

  describe("vfs_umount()");
  assert(vfs_namei("/test1") != NULL, "finds /test1");
  retval = vfs_umount("/test1");
  assert(retval == 0, "detached test3");
  assert(vfs_namei("/test1") == NULL, "does not find /test1 anymore");
  assert(vfs_umount("/test2") != 0, "cannot detach a FS with children");
  end_describe();

  describe("vfs_free()");
  assert(vfs_free(root) == 0, "frees a node");
  end_describe();

  for (uint8_t i = 0; i < NB_NODES; i++) {
    free(nodes_for_testfs[i]);
  }

  return test_summary();
}
