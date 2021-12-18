/** @file */
#ifndef AARCH64_KMAIN_H
#define AARCH64_KMAIN_H

#include <stdint.h>

/**
 * This is the entrypoint of the kernel (C code).
 */
void kmain(uintptr_t w0) __asm__("kmain");

#endif
