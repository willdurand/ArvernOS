#include <stdio.h>

#ifdef __is_libk
#include <drivers/screen.h>

void _putchar(char c) {
    screen_write(c);
}

#else
#include <stdint.h>
#include <sys/syscall.h>

void printf(const char* s) {
    uint64_t i = 0;

    while (s[i] != '\0') {
        write(s[i++]);
    }
}

#endif
