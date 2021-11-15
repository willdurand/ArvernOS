#include <kshell/kshell.h>

#include <fs/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void ls(int argc, char* argv[])
{
  inode_t inode = argc == 1 ? vfs_namei("/") : vfs_namei(argv[1]);

  if (!inode) {
    printf("no such file or directory\n");
    return;
  }

  if (vfs_type(inode) != FS_DIRECTORY) {
    printf("'%s' is not a directory\n", inode->name);
    return;
  }

  uint64_t num = 0;

  while (1) {
    dirent_t* de = vfs_readdir(inode, num++);

    if (!de) {
      break;
    }

    vfs_stat_t stat = { 0 };
    vfs_stat(de->inode, &stat);

    char indicator = ' ';
    if ((stat.mode & S_IFSOCK) == S_IFSOCK) {
      indicator = '=';
    } else if ((stat.mode & S_IFDIR) == S_IFDIR) {
      indicator = '/';
    } else if ((stat.mode & S_IFCHR) == S_IFCHR) {
      indicator = '%';
    }

    printf("%6llu %s%c\n", stat.size, de->name, indicator);

    free(de);
  }
}
