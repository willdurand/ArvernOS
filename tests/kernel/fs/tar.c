#include <fcntl.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <logging.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>

void logging_impl(uint8_t level,
                  uint8_t minimum_level,
                  source_location_t loc,
                  const char* format,
                  ...)
{}

char* get_initrd()
{
  FILE* file = fopen("tests/kernel/test.tar", "rb");

  if (!file) {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  uint64_t size = ftell(file);
  rewind(file);

  char* initrd = malloc(size * sizeof(char));
  fread(initrd, 1, size, file);
  fclose(file);

  return initrd;
}

int main()
{
  char buf[256];

  vfs_init();

  char* initrd = get_initrd();

  if (initrd == NULL) {
    printf("could not get initrd\n");
    return 1;
  }

  describe("tar_fs_create()");
  inode_t root = vfs_mount("/", tar_fs_create((uint64_t)initrd));
  assert(root != NULL, "can be mounted");
  assert(strcmp(root->name, "/") == 0, "is mounted to /");
  assert(vfs_type(root) == FS_DIRECTORY, "is a directory");
  assert(root->data == -1, "is sets data to -1");
  end_describe();

  describe("tar_finddir()");
  inode_t not_found = vfs_namei("/home/will/fileeee");
  assert(not_found == NULL, "returns NULL when file is not found");
  not_found = vfs_namei("/inf");
  assert(not_found == NULL, "returns NULL when partial file name is not found");

  inode_t home = vfs_namei("/home");
  assert(vfs_type(home) == FS_DIRECTORY, "can find a directory");
  assert(home != NULL && strcmp(home->name, "home") == 0,
         "returns a directory with the right name");
  // TODO: fixme
  // home = vfs_namei("/home/");
  // assert(home != NULL && strcmp(home->name, "home") == 0,
  //       "supports trailing slash");

  inode_t will = vfs_namei("/home/will");
  assert(vfs_type(will) == FS_DIRECTORY, "can find a sub-directory");
  assert(will != NULL && strcmp(will->name, "home/will") == 0,
         "returns a sub-directory with the right name");
  end_describe();

  describe("tar_read()");
  uint64_t bytes_read = vfs_read(vfs_namei("/info"), buf, sizeof(buf), 0);
  assert(strncmp(buf, "info file\n", bytes_read) == 0,
         "reads the content of a file");
  assert(bytes_read == 10, "returns the number of bytes read");
  bytes_read = vfs_read(vfs_namei("/info"), buf, sizeof(buf), 5);
  assert(strncmp(buf, "file\n", bytes_read) == 0,
         "reads a file at a given offset");
  assert(bytes_read == 5, "returns the number of bytes read with offset");

  bytes_read = vfs_read(vfs_namei("/home/will/file"), buf, sizeof(buf), 0);
  assert(strncmp(buf, "some content\n", bytes_read) == 0,
         "reads the content of a file in a sub-directory");
  assert(bytes_read == 13, "returns the number of bytes read");
  end_describe();

  dirent_t* de = NULL;
  describe("tar_readdir() with mountpoint");
  de = vfs_readdir(root, 0);
  assert(strcmp(de->name, ".") == 0, "returns the current directory");
  assert(de->inode == root, "returns the same inode");
  free(de);

  de = vfs_readdir(root, 1);
  assert(strcmp(de->name, "..") == 0, "returns the parent directory");
  assert(de->inode == root->parent, "returns the parent inode");
  free(de);

  de = vfs_readdir(root, 2);
  assert(strcmp(de->name, "home") == 0,
         "returns the first entry in the TAR file");
  free(de);

  de = vfs_readdir(root, 3);
  assert(strcmp(de->name, "info") == 0, "returns the second direct child of /");
  free(de);
  end_describe();

  describe("tar_readdir() with a directory");
  de = vfs_readdir(home, 0);
  assert(strcmp(de->name, ".") == 0, "returns the current directory");
  assert(de->inode == home, "returns the same inode");
  free(de);

  de = vfs_readdir(home, 1);
  assert(strcmp(de->name, "..") == 0, "returns the parent directory");
  assert(de->inode == home->parent, "returns the parent inode");
  free(de);

  de = vfs_readdir(home, 2);
  assert(strcmp(de->name, "will") == 0,
         "returns the first direct child in /home/");
  free(de);
  end_describe();

  describe("tar_readdir() with a sub-directory");
  de = vfs_readdir(will, 2);
  assert(strcmp(de->name, "file") == 0,
         "returns the first direct child in /home/will/");
  assert(vfs_type(de->inode) == FS_FILE, "returns a file");
  free(de);
  end_describe();

  describe("tar_stat()");
  vfs_stat_t stat = { 0 };
  vfs_stat(vfs_namei("/home/will/file"), &stat);
  assert(stat.size == 13, "sets the file size");

  vfs_stat(vfs_namei("/home/will"), &stat);
  assert(stat.size == 0, "sets a size of zero when inode is a directory");
  end_describe();

  describe("tar_isatty()");
  assert(vfs_isatty(root) == 0, "returns 0");
  end_describe();

  vfs_free(root);
  free(initrd);

  return test_summary();
}
