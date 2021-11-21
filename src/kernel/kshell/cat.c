#include <kshell/kshell.h>

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

    if (vfs_type(f) != FS_FILE) {
      printf("'%s' is not a printable file (type=%X)\n", f->name, vfs_type(f));
      vfs_free(f);
      continue;
    }

    char buf[512];
    for (int offset = 0, bytes_read = 0;
         (bytes_read = vfs_read(f, buf, sizeof(buf), offset)) > 0;
         offset += bytes_read) {
      printf("%.*s", bytes_read, buf);
    }

    vfs_free(f);
  }
}
