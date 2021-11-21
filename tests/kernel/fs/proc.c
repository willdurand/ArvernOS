#include <fs/proc.h>
#include <fs/vfs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

#define BUF_SIZE 20

int proc_read_version(char* buf, size_t size)
{
  snprintf(buf, size, "some version\n");

  return 0;
}

int main()
{
  vfs_init();

  char buf[BUF_SIZE];

  inode_t root, version;

  describe("proc_fs_create()");
  root = vfs_mount("/", proc_fs_create());
  assert(root != 0, "can be mounted");
  assert(vfs_type(root) == FS_DIRECTORY, "is a directory");
  end_describe();

  describe("proc_isatty()");
  assert(vfs_isatty(root) == 0, "returns 0");
  end_describe();

  describe("proc_fs_create_readonly_file()");
  // Add a pseudo-file that will be used by the next test cases.
  vfs_mount("/version",
            proc_fs_create_readonly_file("version", &proc_read_version));
  end_describe();

  describe("proc_readdir()");
  dirent_t* de = NULL;
  de = vfs_readdir(root, 0);
  assert(strcmp(de->name, ".") == 0, "returns the current directory");
  assert(de->inode == root, "returns the same inode");
  free(de);

  de = vfs_readdir(root, 1);
  assert(strcmp(de->name, "..") == 0, "returns the parent directory");
  assert(de->inode == root->parent, "returns the parent inode");
  free(de);

  de = vfs_readdir(root, 2);
  assert(strcmp(de->name, "version") == 0,
         "returns the version file in position #2");
  free(de);
  end_describe();

  describe("proc_finddir()");
  inode_t not_found = vfs_namei("/file-not-found");
  assert(not_found == NULL, "returns NULL when file is not found");

  version = vfs_namei("/version");
  assert(vfs_type(version) == FS_FILE, "finds the version file");

  inode_t not_version = vfs_namei("/versionn");
  assert(not_version == NULL, "returns NULL when file is not found");
  end_describe();

  describe("proc_read()");
  version = vfs_namei("/version");
  uint64_t bytes_read = vfs_read(version, buf, BUF_SIZE, 0);
  assert(strcmp(buf, "some version\n") == 0,
         "reads the content of the version file");
  assert(bytes_read == 13, "returns the number of bytes read");

  bytes_read = vfs_read(version, buf, BUF_SIZE, 1);
  assert(strcmp(buf, "ome version\n") == 0,
         "reads the content of the version file with offset");
  assert(bytes_read == 12, "returns the number of bytes read with offset");

  bytes_read = vfs_read(version, buf, BUF_SIZE, 30);
  assert(strcmp(buf, "") == 0, "handles big offsets");
  assert(bytes_read == 0, "returns no bytes read when offset is too big");
  end_describe();

  vfs_free(root);

  return test_summary();
}
