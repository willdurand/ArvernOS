/** @file */
#ifndef AARCH64_KMAIN_H
#define AARCH64_KMAIN_H

/**
 * This is the entrypoint of the kernel (C code).
 */
void kmain() __asm__("kmain");

#endif
