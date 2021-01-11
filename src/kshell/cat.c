#include "kshell.h"
#include <fs/vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void cat(int argc, char* argv[])
{
  if (argc < 2) {
    printf("%s requires an argument\n", argv[0]);
  }

  for (int i = 1; i < argc; i++) {
    inode_t f = vfs_namei(argv[i]);

    if (!f) {
      printf("no such file or directory\n");
      continue;
    }

    if (f->type != FS_FILE) {
      printf("'%s' is not a printable file\n", f->name);
      vfs_free(f);
      continue;
    }

    char buf[512];
    vfs_read(f, buf, sizeof(buf), 0);
    printf("%s", buf);
    vfs_free(f);
  }
}
