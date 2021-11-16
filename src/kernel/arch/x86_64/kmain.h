/** @file */
#ifndef KMAIN_H
#define KMAIN_H

#include <stdint.h>

/**
 * This is the entrypoint of the kernel (C code).
 *
 * @param addr the multiboot info address
 */
void kmain(uint64_t addr) __asm__("kmain");

#endif
