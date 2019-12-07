#include <stdio.h>
#include <drivers/screen.h>

#ifdef __is_libk

void _putchar(char c) {
    screen_write(c);
}

#else

void _putchar(char c) {
    // TODO: add syscall to write to the screen.
}

#endif
