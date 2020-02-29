#include "init.h"
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

void uptime()
{
  int fd = open("/proc/uptime", 0); // O_RDONLY

  char buf[20];
  if (read(fd, buf, 20) > 0) {
    // Remove `\n` in the content read from /proc/uptime
    buf[strlen(buf) - 1] = '\0';
    printf("up %s seconds\n", buf);
  } else {
    printf("failed to read /proc/uptime\n");
  }

  close(fd);
}
