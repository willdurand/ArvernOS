/** @file */
#ifndef ARCH_KERNEL_H
#define ARCH_KERNEL_H

/**
 * Shutdown everything and perform a clean reboot.
 */
void arch_restart();

/**
 * Shutdown everything and perform a clean system power off.
 */
void arch_poweroff();

/**
 * hutdown everything and perform a clean system halt.
 */
void arch_halt();

#endif
