#ifndef CORE_PANIC_H
#define CORE_PANIC_H

#include <core/debug.h>

#define __PANIC(format, ...)    DEBUG(format "%s", __VA_ARGS__); \
                                kernel_panic("\nPANIC in %s() (line %d): " \
                                format "%s", \
                                __func__, __LINE__, __VA_ARGS__);
#define PANIC(...)              __PANIC(__VA_ARGS__, "\n")

void kernel_panic(const char* format, ...);

#endif
