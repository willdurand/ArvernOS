/**
 * @file
 * @see https://en.wikipedia.org/wiki/Kernel_panic
 *
 * This file mainly contains the macros to use for kernel panics.
 */
#ifndef PANIC_H
#define PANIC_H

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

/// This macro is similar to `PANIC()` but it won't attempt to print a
/// stacktrace.
#define PANIC_NO_STACKTRACE(...) __PANIC(false, __VA_ARGS__, "\n")

/**
 * Handles a kernel panic.
 *
 * @param dump_stacktrace whether to dump a stacktrace
 * @param format the reason for the kernel panic
 */
void kernel_panic(bool dump_stacktrace, const char* format, ...);

/**
 * Dumps a stacktrace on both the standard and debug outputs (unless both are
 * the same, in which case we do not print the debug messages).
 */
void kernel_dump_stacktrace();

#endif
