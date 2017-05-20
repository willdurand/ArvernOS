#ifndef KMAIN_H
#define KMAIN_H

#include <core/boot.h>

#define KERNEL_ASCII    "             _  __\n" \
                        "    o  |  | / \\(_ \n" \
                        "\\^/ |  |  | \\_/__)\n"

#define KERNEL_NAME    "willOS"
#define KERNEL_VERSION "0.0.1"
#define KERNEL_DATE     __DATE__
#define KERNEL_TIME     __TIME__

void kmain(unsigned long magic, unsigned long addr) __asm__("kmain");

#endif
