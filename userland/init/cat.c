#include "init.h"
#include <stdio.h>
#include <sys/syscall.h>

#define BUF_SIZE 512

void cat(char* arg)
{
  if (arg == NULL) {
    printf("usage:\n  cat <file>\n");
    return;
  }

  int fd = open(arg, 0); // O_RDONLY

  char buf[BUF_SIZE];
  if (read(fd, buf, BUF_SIZE) > 0) {
    printf("%s\n", buf);
  } else {
    printf("failed to open %s\n", arg);
    return;
  }

  close(fd);
}
