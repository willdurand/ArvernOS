#include <fcntl.h>
#include <stdio.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  printf("==== userland test suite ====\n");

#ifndef __arvernos__
  printf("This program should be compiled for and executed on ArvernOS\n");
  return 1;
#else
  // open/write/close syscalls
  int fd = open("/dev/null", O_WRONLY);
  int bytes_written = write(fd, NULL, 50);
  close(fd);

  if (bytes_written != 50) {
    printf("  write() failed: %d bytes written\n", bytes_written);
    reboot(REBOOT_CMD_POWER_OFF_EXIT_CODE_1);
    return 1;
  }

  // fork/waitpid syscalls
  pid_t pid = fork();
  if (pid < 0) {
    printf("  fork() failed: %d\n", (int)pid);
    reboot(REBOOT_CMD_POWER_OFF_EXIT_CODE_1);
    return 1;
  }

  if (pid == 0) {
    // This is the child process. Exit with a predictable status code.
    printf("  fork() - child (pid=%d) exiting with 42\n", (int)getpid());
    exit(42);
  }

  // This is the parent process. Wait for the child and check its exit status.
  printf("  fork() - parent forked child pid=%d\n", (int)pid);

  int status = 0;
  pid_t reaped = waitpid(pid, &status);

  if (reaped != pid) {
    printf("  waitpid() failed: %d\n", (int)reaped);
    reboot(REBOOT_CMD_POWER_OFF_EXIT_CODE_1);
    return 1;
  }

  if (status != 42) {
    printf("  waitpid() - unexpected exit status: %d\n", status);
    reboot(REBOOT_CMD_POWER_OFF_EXIT_CODE_1);
    return 1;
  }

  printf("Looks like everything is alright!\n");
  reboot(REBOOT_CMD_POWER_OFF_EXIT_CODE_0);

  return 0;
#endif
}
