#include <fs/proc.h>
#include <fs/vfs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

#define BUF_SIZE 20

// This is a fake implementation.
uint64_t timer_uptime()
{
  return 123;
}

int main()
{
  vfs_init();

  char buf[BUF_SIZE];

  inode_t root, uptime, hostname;

  describe("proc_fs_init()");
  root = vfs_mount("/", proc_fs_init());
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
  free(de->inode);
  free(de);
  de = vfs_readdir(root, 3);
  assert(strcmp(de->name, "version") == 0,
         "returns the version file in position #3");
  free(de->inode);
  free(de);
  de = vfs_readdir(root, 4);
  assert(strcmp(de->name, "hostname") == 0,
         "returns the hostname file in position #4");
  free(de->inode);
  free(de);
  end_describe();

  describe("proc_finddir()");
  inode_t not_found = vfs_namei("/file-not-found");
  assert(not_found == 0, "returns 0 when file is not found");
  uptime = vfs_namei("/uptime");
  assert(vfs_inode_type(uptime) == FS_FILE, "finds the uptime file");
  free(uptime);
  inode_t not_uptime = vfs_namei("/uptimee");
  assert(not_uptime == 0, "returns 0 when file is not found");
  end_describe();

  describe("proc_isatty()");
  assert(vfs_isatty(root) == 0, "returns 0");
  end_describe();

  describe("proc_read()");
  uptime = vfs_namei("/uptime");
  uint64_t bytes_read = vfs_read(uptime, &buf, BUF_SIZE, 0);
  assert(strcmp(buf, "123\n") == 0, "reads the content of the uptime file");
  assert(bytes_read == 4, "returns the number of bytes read");
  bytes_read = vfs_read(uptime, &buf, BUF_SIZE, 1);
  assert(strcmp(buf, "23\n") == 0,
         "reads the content of the uptime file with offset");
  assert(bytes_read == 3, "returns the number of bytes read with offset");
  bytes_read = vfs_read(uptime, &buf, BUF_SIZE, 4);
  assert(strcmp(buf, "") == 0, "handles big offsets");
  assert(bytes_read == 0, "returns no bytes read when offset is too big");
  free(uptime);
  end_describe();

  describe("proc_stat()");
  hostname = vfs_namei("/hostname");
  uptime = vfs_namei("/uptime");
  stat_t stat;
  vfs_stat(uptime, &stat);
  assert(stat.size == 1, "sets the size to 1 by default");
  vfs_stat(hostname, &stat);
  assert(stat.size > 0, "sets the correct size for hostname");
  free(uptime);
  free(hostname);
  end_describe();

  describe("proc_write()");
  char* new_hostname = "new-hostname";
  hostname = vfs_namei("/hostname");
  uptime = vfs_namei("/uptime");
  uint64_t bytes_written =
    vfs_write(hostname, new_hostname, strlen(new_hostname), 0);
  vfs_read(hostname, &buf, BUF_SIZE, 0);
  // Remove the trailing newline.
  buf[strlen(buf) - 1] = '\0';
  assert(strcmp(buf, new_hostname) == 0, "writes the new hostname value");
  assert(bytes_written == strlen(new_hostname),
         "returns the number of bytes written");
  char* long_hostname = "this-is-a-super-looooooong-host-name";
  bytes_written = vfs_write(hostname, long_hostname, strlen(long_hostname), 0);
  assert(bytes_written == strlen(long_hostname),
         "reallocs memory to deal with long hostname values");
  bytes_written = vfs_write(uptime, new_hostname, strlen(new_hostname), 0);
  assert(bytes_written == 0, "does not allow write to uptime");
  free(uptime);
  free(hostname);
  end_describe();

  proc_fs_deinit();
  free(root);

  return test_summary();
}
