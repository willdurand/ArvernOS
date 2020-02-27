#include <fs/proc.h>
#include <fs/vfs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

// This is a fake implementation.
uint64_t timer_uptime()
{
  return 123;
}

int main()
{
  vfs_init();

  describe("proc_fs_init()");
  inode_t root = vfs_mount("/", proc_fs_init());
  assert(root != 0, "can be mounted");
  assert(vfs_inode_type(root) == FS_DIRECTORY, "is a directory");
  end_describe();

  describe("proc_readdir()");
  dirent_t* de;
  de = vfs_readdir(root, 0);
  assert(strcmp(de->name, ".") == 0, "returns the current directory");
  assert(de->inode == root, "returns the same inode");
  free(de);
  de = vfs_readdir(root, 1);
  assert(strcmp(de->name, "..") == 0, "returns the parent directory");
  assert(de->inode == root->parent, "returns the parent inode");
  free(de);
  de = vfs_readdir(root, 2);
  assert(strcmp(de->name, "uptime") == 0,
         "returns the uptime file in position #2");
  free(de);
  end_describe();

  describe("proc_finddir()");
  inode_t not_found = vfs_namei("/file-not-found");
  assert(not_found == 0, "returns 0 when file is not found");
  inode_t uptime = vfs_namei("/uptime");
  assert(vfs_inode_type(uptime) == FS_FILE, "finds the uptime file");
  free(uptime);
  end_describe();

  describe("proc_isatty()");
  assert(vfs_isatty(root) == 0, "returns 0");
  end_describe();

  describe("proc_read()");
  char buf[20];
  uint64_t bytes_read = vfs_read(vfs_namei("/uptime"), &buf, sizeof(buf), 0);
  assert(strcmp(buf, "123\n") == 0, "reads the content of the uptime file");
  assert(bytes_read == 4, "returns the number of bytes read");
  bytes_read = vfs_read(vfs_namei("/uptime"), &buf, sizeof(buf), 1);
  assert(strcmp(buf, "23\n") == 0,
         "reads the content of the uptime file with offset");
  assert(bytes_read == 3, "returns the number of bytes read with offset");
  bytes_read = vfs_read(vfs_namei("/uptime"), &buf, sizeof(buf), 4);
  assert(strcmp(buf, "") == 0, "handles big offsets");
  assert(bytes_read == 0, "returns no bytes read when offset is too big");
  end_describe();

  describe("proc_stat()");
  stat_t stat;
  vfs_stat(vfs_namei("/uptime"), &stat);
  assert(stat.size == 0, "sets the size to 0");
  end_describe();

  free(root);

  return test_summary();
}
