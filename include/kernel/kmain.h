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
 * Initializes the common filesystem code. The first argument (`addr`) should
 * point to an init ramdisk (or be `NULL` when there is no ramdisk available).
 *
 * @param addr the address of the init ramdisk (or `NULL`)
 */
void kmain_init_fs(uintptr_t addr);

/**
 * Starts the kernel. This function should be called once the arch- and/or
 * board-specific initialization code has been executed.
 *
 * @param cmdline the kernel command line, usually passed by the bootloader
 * somehow
 */
void kmain_start(const char* cmdline);

// utils
void print_ko();
void print_ok();
void print_step(const char* msg);

#endif
