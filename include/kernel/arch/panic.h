/**
 * @file
 *
 * Architecture-specific implementation to load debug symbols in order to print
 * stacktraces in case of a kernel panic.
 */
#ifndef ARCH_PANIC_H
#define ARCH_PANIC_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Loads symbols used in stacktraces.
 *
 * @param addr the address of the symbols data
 * @param size the size of the symbols data
 */
void arch_kernel_load_symbols(uint64_t addr, uint64_t size);

/**
 * Prints a stacktrace.
 */
void arch_kernel_dump_stacktrace();

#endif
