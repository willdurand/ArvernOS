#include "shell.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_selftest_header(const char* name);

void print_selftest_header(const char* name)
{
  printf("\n\033[1;33m[%s]\033[0m\n", name);
}

void selftest()
{
#ifdef __arvernos__
  print_selftest_header("syscall");
  printf("  syscalling\n");
  int retval = test("usermode");
  if (retval != 42) {
    printf("Got unexpected syscall return value: %d (expected: 42)\n", retval);
  }
#endif

  print_selftest_header("filesystem");
  int fd = open("/dev/debug", O_WRONLY);

  if (fd < 0) {
    printf("could not open /dev/debug (errno=%d)\n", errno);
  } else {
    const char* message = "  this message should be written to the console\n";
    if (write(fd, message, strlen(message)) < 0) {
      printf("failed to write to /dev/debug (errno=%d)\n", errno);
    }

    close(fd);
  }

  printf("\ndone.\n");
}
