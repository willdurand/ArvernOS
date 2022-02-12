#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __arvernos__
#define HOSTNAME_FILE "/proc/hostname"
#else
#define HOSTNAME_FILE "/proc/sys/kernel/hostname"
#endif

int main(int argc, char* argv[])
{
  int retval = 0;
  int fd = open(HOSTNAME_FILE, O_RDWR);

  if (fd < 0) {
    printf("could not open %s (errno=%d)\n", HOSTNAME_FILE, errno);
    return 1;
  }

  if (argc == 1) {
    char buf[50];
    if (read(fd, buf, 50) > 0) {
      printf("%s", buf);
    } else {
      printf("failed to read from %s (errno=%d)\n", HOSTNAME_FILE, errno);
      retval = 2;
    }
  } else {
    if (write(fd, argv[1], strlen(argv[1])) > 0) {
      printf("hostname updated\n");
    } else {
      printf("failed to write to %s (errno=%d)\n", HOSTNAME_FILE, errno);
      retval = 3;
    }
  }

  close(fd);

  return retval;
}
