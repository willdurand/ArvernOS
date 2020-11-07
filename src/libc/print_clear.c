#include <stdio.h>

#ifdef __is_libc
#include <proc/fd.h>
#include <sys/syscall.h>

void print_clear()
{
  write(FD_STDOUT, ESCAPE_SEQUENCE_CLEAR, strlen(ESCAPE_SEQUENCE_CLEAR));
}

#endif
