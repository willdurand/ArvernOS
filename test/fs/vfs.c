#include <fs/vfs.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

int main()
{
  vfs_init();

  describe("vfs_namei");
  inode_t root = vfs_namei("/");
  assert(root != 0, "creates a valid inode for /");
  assert(strcmp(root->name, "/") == 0, "has a valid name");
  assert(root->parent == root, "has a parent set to itself");
  end_describe();

  describe("vfs_free");
  vfs_free(root);
  assert(root != 0, "does not free a used inode");
  end_describe();

  free(root);

  return test_summary();
}
