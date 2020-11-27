#include "shell.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef __is_libc
#include <sys/syscall.h>

#define HOSTNAME_FILE "/proc/hostname"
#else
#include <fcntl.h>
#include <unistd.h>

#define HOSTNAME_FILE "/proc/sys/kernel/hostname"
#endif

void hostname(char* arg)
{
  int fd = open(HOSTNAME_FILE, 02); // O_RDWR

  if (arg == NULL) {
    char buf[50];
    if (read(fd, buf, 50) > 0) {
      printf("%s", buf);
    } else {
      printf("failed to read from %s (errno=%d)\n", HOSTNAME_FILE, errno);
    }
  } else {
    if (write(fd, arg, strlen(arg)) > 0) {
      printf("hostname updated\n");
    } else {
      printf("failed to write to %s (errno=%d)\n", HOSTNAME_FILE, errno);
    }
  }

  close(fd);
}
