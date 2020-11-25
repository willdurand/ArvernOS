#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __is_libc
#include <sys/syscall.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

int main()
{
  int exit_code = 0;
  int fd = open("/proc/meminfo", 0); // O_RDONLY

  char buf[256];
  if (read(fd, buf, 256) > 0) {
    printf("%s", buf);
  } else {
    printf("failed to read /proc/meminfo\n");
    exit_code = 1;
  }

  close(fd);

  return exit_code;
}
