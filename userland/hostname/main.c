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

int hostname(int argc, char* argv[])
{
  int retval = 0;
  int fd = open(HOSTNAME_FILE, 02); // O_RDWR

  if (argc == 1) {
    char buf[50];
    if (read(fd, buf, 50) > 0) {
      printf("%s", buf);
    } else {
      printf("failed to read from %s (errno=%d)\n", HOSTNAME_FILE, errno);
      retval = -1;
    }
  } else {
    if (write(fd, argv[1], strlen(argv[1])) > 0) {
      printf("hostname updated\n");
    } else {
      printf("failed to write to %s (errno=%d)\n", HOSTNAME_FILE, errno);
      retval = -1;
    }
  }

  close(fd);

  return retval;
}
