/**
 * @file
 *
 * This file contains the functions shared by all arch- and/or board-specific
 * `kmain()` functions to start the kernel in ArvernOS.
 */
#ifndef KMAIN_START_H
#define KMAIN_START_H

#include <boot.h>
#include <stdint.h>

/**
 * Early starts the kernel.
 */
void kmain_early_start();

/**
 * Starts the kernel. This function should be called once the arch- and/or
 * board-specific initialization code has been executed.
 *
 * The boot information (command line, initial ramdisk, etc.) must have been
 * recorded with `boot_info_set()` beforehand (cf. `boot.h`).
 *
 * @param info the boot information discovered by the arch-specific boot code
 */
void kmain_start(const boot_info_t* info);

#endif
