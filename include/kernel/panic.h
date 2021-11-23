/**
 * @file
 * @see https://en.wikipedia.org/wiki/Kernel_panic
 *
 * This file contains the macros to use for kernel panics. The implementation
 * relies on some architecture-specific code to dump stack traces (optional but
 * nice to have) and halt the system.
 */
#ifndef PANIC_H
#define PANIC_H

#include <arch/panic.h>
#include <logging.h>
#include <stdbool.h>

#define __PANIC(dump_stacktrace, format, ...)                                  \
  DEBUG(format "%s", __VA_ARGS__);                                             \
  kernel_panic(dump_stacktrace,                                                \
               "\nPANIC in %s() (line %d): " format "%s",                      \
               __func__,                                                       \
               __LINE__,                                                       \
               __VA_ARGS__);

/// This macro should only be used when something goes terribly wrong.
#define PANIC(...) __PANIC(true, __VA_ARGS__, "\n")

#define PANIC_NO_STACKTRACE(...) __PANIC(false, __VA_ARGS__, "\n")

/**
 * Handles a kernel panic.
 *
 * @param dump_stacktrace whether to dump a stacktrace
 * @param format the reason for the kernel panic
 */
void kernel_panic(bool dump_stacktrace, const char* format, ...);

#endif
