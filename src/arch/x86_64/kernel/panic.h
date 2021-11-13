/** @file */
#ifndef CORE_PANIC_H
#define CORE_PANIC_H

#include <logging.h>
#include <stdbool.h>
#include <stdint.h>

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

typedef struct stack_frame
{
  struct stack_frame* rbp;
  uint64_t rip;
} stack_frame_t;

/**
 * Handles a kernel panic.
 *
 * @param dump_stacktrace whether to dump a stacktrace
 * @param format the reason for the kernel panic
 */
void kernel_panic(bool dump_stacktrace, const char* format, ...);

/**
 * Loads symbols used in stacktraces.
 *
 * @param addr the address of the symbols data
 * @param size the size of the symbols data
 */
void kernel_load_symbols(uint64_t addr, uint64_t size);

/**
 * Prints a stacktrace.
 */
void kernel_dump_stacktrace();

#endif
