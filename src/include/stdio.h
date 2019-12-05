#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

void printf(const char* format, ...);

#ifdef __is_libk
// This section is for the kernel version of `stdio.h`.

#define DEVICE_SCREEN 0

void vprintf(int device, const char* format, va_list arg);

#endif

#endif
