/** @file */
#ifndef KMAIN_H
#define KMAIN_H

#include <core/boot.h>
#include <stdint.h>

#define KERNEL_ASCII                                                           \
  "               _  __\n"                                                     \
  "      o  |  | / \\(_ \n"                                                    \
  "  \\^/ |  |  | \\_/__)\n"

#ifndef KERNEL_NAME
#define KERNEL_NAME "noname OS"
#endif

#ifdef ENABLE_KERNEL_DEBUG
#define KERNEL_VERSION "DEBUG MODE"
#else
#define KERNEL_VERSION "0.0.2"
#endif

#define KERNEL_DATE __DATE__
#define KERNEL_TIME __TIME__

#ifndef GIT_HASH
#define GIT_HASH "dirty"
#endif

/**
 * This is the entrypoint of the kernel (C code).
 *
 * @param addr the multiboot info address
 */
void kmain(uint64_t addr) __asm__("kmain");

#endif
