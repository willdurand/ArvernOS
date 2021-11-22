#include <fs/sock.h>
#include <fs/vfs.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

#define BUF_SIZE 128

int main()
{
  vfs_init();

  vfs_stat_t st = { 0 };
  char buf[BUF_SIZE];

  describe("sock_fs_create()");
  inode_t root = vfs_mount("/", sock_fs_create());
  assert(root != 0, "can be mounted");
  assert(vfs_type(root) == FS_DIRECTORY, "is a directory");
  end_describe();

  describe("sock_create()");
  inode_t node1 = vfs_create(root, NULL, 0);
  assert(vfs_type(node1) == FS_FILE, "creates a file");
  assert(strcmp(node1->name, "1.sock") == 0, "generates a name");
  assert(node1->parent == root, "sets the correct parent");
  assert(node1->data == 0, "stores the entry index");
  end_describe();

  describe("sock_stat()");
  vfs_stat(root, &st);
  assert(st.size == 1,
         "sets the size of the FS root to the number of used entries");
  vfs_stat(node1, &st);
  assert(st.size == 0, "sets the size of a node to the length of the buffer");
  end_describe();

  describe("sock_write()");
  sprintf(buf, "%s", "some data");
  vfs_write(node1, buf, strlen(buf), 0);
  vfs_stat(node1, &st);
  assert(st.size == strlen(buf), "adds data to the sock entry buffer");
  end_describe();

  describe("sock_read()");
  memset(buf, 0, BUF_SIZE);
  uint64_t bytes_read = vfs_read(node1, buf, 4, 0);
  assert(bytes_read == 4, "returns the number of bytes read");
  assert(strcmp(buf, "some") == 0, "returns data in the receiving buffer");
  end_describe();

  describe("sock_close()");
  vfs_close(node1);
  vfs_stat(root, &st);
  assert(st.size == 0, "should have removed the entry for node1");
  end_describe();

  describe("vfs_free()");
  // Without this, `vfs_free()` won't do anything.
  root->parent = NULL;
  assert(vfs_free(root) == 0, "frees the root node");
  end_describe();

  return test_summary();
}
