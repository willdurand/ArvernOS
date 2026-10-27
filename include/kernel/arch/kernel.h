/** @file */
#ifndef ARCH_KERNEL_H
#define ARCH_KERNEL_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Shutdowns everything and performs a clean reboot.
 */
void arch_restart();

/**
 * Shutdowns everything and performs a clean system power off.
 *
 * @param exit_code when `CONFIG_SEMIHOSTING` is set to `1`, this value is
 * returned to the emulator. Otherwise, this value is ignored
 */
void arch_poweroff(int exit_code);

/**
 * Shutdowns everything and performs a clean system halt.
 */
void arch_halt();

#endif
