/** @file */
#ifndef AARCH32_KMAIN_H
#define AARCH32_KMAIN_H

#include <stdint.h>

/**
 * This is the entrypoint of the kernel (C code).
 */
void kmain(uint32_t r0, uint32_t r1, uint32_t r2) __asm__("kmain");

#endif
