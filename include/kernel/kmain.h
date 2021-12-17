/**
 * @file
 *
 * This file contains the functions shared by all arch- and/or board-specific
 * `kmain()` functions, which are the kernel entry point functions in
 * ArvernOS.
 */
#ifndef KMAIN_H
#define KMAIN_H

#include <stdint.h>

/**
 * Early starts the kernel.
 */
void kmain_early_start();

/**
 * Starts the kernel. This function should be called once the arch- and/or
 * board-specific initialization code has been executed.
 *
 * @param initrd_addr the address of the init ramdisk (or `NULL`)
 * @param cmdline the kernel command line, usually passed by the bootloader
 * somehow
 */
void kmain_start(uintptr_t initrd_addr, const char* cmdline);

#endif
