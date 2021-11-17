#ifndef ARCH_PANIC_H
#define ARCH_PANIC_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Handles a kernel panic.
 *
 * @param dump_stacktrace whether to dump a stacktrace
 * @param format the reason for the kernel panic
 */
void arch_kernel_panic(bool dump_stacktrace, const char* format, ...);

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
