#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __is_libc
#include <sys/syscall.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("usage: %s file\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], 0);

    if (fd < 3) {
      printf("could not open: %s\n", argv[i]);
      continue;
    }

    char buf[1024];
    if (read(fd, buf, 1024) > 0) {
      printf("%s", buf);
    } else {
      printf("could not read: %s\n", argv[i]);
    }

    close(fd);
  }

  return 0;
}
