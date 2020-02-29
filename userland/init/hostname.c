#include "init.h"
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

void hostname(const char* readline)
{
  // The first call returns the command name (a.k.a. `hostname`).
  char* token = strtok(readline, " ");
  // The second call returns the first argument or `NULL`.
  token = strtok(NULL, " ");

  int fd = open("/proc/hostname", 02); // O_RDWR

  if (token == NULL) {
    char buf[50];
    if (read(fd, buf, 50) > 0) {
      // Remove `\n` in the content read from /proc/hostname
      buf[strlen(buf) - 1] = '\0';
      printf("%s\n", buf);
    } else {
      printf("failed to read from /proc/hostname\n");
    }
  } else {
    if (write(fd, token, strlen(token)) > 0) {
      printf("hostname updated\n");
    } else {
      printf("failed to write to /proc/hostname\n");
    }
  }

  close(fd);
}
