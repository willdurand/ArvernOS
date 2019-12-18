#include <stdio.h>

#ifdef __is_libc
#include <sys/syscall.h>

void putchar(char c) {
    write(c);
}
#endif
