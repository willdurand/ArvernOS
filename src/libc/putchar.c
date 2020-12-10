#include <stdio.h>

#ifdef __is_libc
#include <proc/descriptor.h>
#include <sys/syscall.h>

void putchar(char c)
{
  write(STDOUT, &c, 1);
}
#endif
