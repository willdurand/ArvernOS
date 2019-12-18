#include <stdio.h>

#ifndef __is_libk
#include <sys/syscall.h>

void putchar(char c) {
    write(c);
}
#endif
