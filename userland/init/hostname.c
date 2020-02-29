#include "init.h"
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

void hostname(char* arg)
{
  int fd = open("/proc/hostname", 02); // O_RDWR

  if (arg == NULL) {
    char buf[50];
    if (read(fd, buf, 50) > 0) {
      // Remove `\n` in the content read from /proc/hostname
      buf[strlen(buf) - 1] = '\0';
      printf("%s\n", buf);
    } else {
      printf("failed to read from /proc/hostname\n");
    }
  } else {
    if (write(fd, arg, strlen(arg)) > 0) {
      printf("hostname updated\n");
    } else {
      printf("failed to write to /proc/hostname\n");
    }
  }

  close(fd);
}
