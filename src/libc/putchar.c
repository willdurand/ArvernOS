#include <stdio.h>

#include <proc/descriptor.h>
#include <sys/syscall.h>

void putchar(char c)
{
  // Make sure we write a null-terminated string.
  write(STDOUT, (char[]){ c, '\0' }, 1);
}
