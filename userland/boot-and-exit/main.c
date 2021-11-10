#include <fcntl.h>
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <unistd.h>

int main()
{
  // Invoke some syscalls.
  int fd = open("/dev/null", O_WRONLY);
  int bytes_written = write(fd, NULL, 50);
  close(fd);

  if (bytes_written == 50) {
    printf("Looks like everything is alright!\n");

#ifdef __willos__
    reboot(REBOOT_CMD_POWER_OFF);
#endif

    return 0;
  }

  return 1;
}
