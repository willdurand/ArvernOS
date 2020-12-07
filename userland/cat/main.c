#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("usage: %s file\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);

    if (fd < 3) {
      printf("could not open: %s\n", argv[i]);
      continue;
    }

    char buf[1025];
    int bytes_read = 0;
    if ((bytes_read = read(fd, buf, sizeof(buf) - 1)) > 0) {
      buf[bytes_read] = '\0';
      printf("%s", buf);
    } else {
      printf("could not read: %s\n", argv[i]);
    }

    close(fd);
  }

  return 0;
}
