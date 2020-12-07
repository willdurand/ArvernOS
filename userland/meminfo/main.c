#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
  int retval = 0;
  int fd = open("/proc/meminfo", O_RDONLY);

  char buf[20];
  while (read(fd, buf, 20) > 0) {
    printf("%s", buf);
    memset(buf, 0, 20);
  }

  close(fd);

  return retval;
}
