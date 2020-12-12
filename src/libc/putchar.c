#include <stdio.h>

#include <proc/descriptor.h>
#include <sys/syscall.h>

void putchar(char c)
{
  write(STDOUT, &c, 1);
}
