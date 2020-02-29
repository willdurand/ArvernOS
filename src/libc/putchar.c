#include <stdio.h>

#ifdef __is_libc
#include <proc/fd.h>
#include <sys/syscall.h>

void putchar(char c)
{
  write(FD_STDOUT, &c, 1);
}
#endif
