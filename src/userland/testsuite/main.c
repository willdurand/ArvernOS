#include <fcntl.h>
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <unistd.h>

int main()
{
  printf("==== userland test suite ====");

#ifndef __willos__
  printf("This program should be compiled for and executed on willOS\n");
  return 1;
#else
  // Invoke some syscalls.
  int fd = open("/dev/null", O_WRONLY);
  int bytes_written = write(fd, NULL, 50);
  close(fd);

  if (bytes_written == 50) {
    printf("Looks like everything is alright!\n");

    reboot(REBOOT_CMD_POWER_OFF);

    return 0;
  }

  return 1;
#endif
}
