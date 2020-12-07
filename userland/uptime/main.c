#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __is_libc
#include <sys/syscall.h>
#else
#include <unistd.h>
#endif

int main()
{
  int fd = open("/proc/uptime", O_RDONLY);

  char buf[20];
  if (read(fd, buf, 20) > 0) {
    char* p = strchr(buf, ' ');

    if (p) {
      // Cut the string at the first space.
      *p = '\0';
    } else {
      // Remove `\n` in the content read from /proc/uptime
      buf[strlen(buf) - 1] = '\0';
    }

    int seconds = atoi(buf);
    int d, h, m, s;

    d = (seconds / 3600) / 24;
    h = (seconds / 3600) - (d * 24);
    m = (seconds - (3600 * (h + d * 24))) / 60;
    s = (seconds - (3600 * (h + d * 24)) - (m * 60));

    if (d == 0 && h == 0) {
      printf("up %d minutes, %d seconds\n", m, s);
    } else if (d == 0) {
      printf("up %d hours, %d minutes, %d seconds\n", h, m, s);
    } else {
      printf("up %d days, %d hours, %d minutes, %d seconds\n", d, h, m, s);
    }
  } else {
    printf("failed to read /proc/uptime\n");
    return 1;
  }

  close(fd);

  return 0;
}
