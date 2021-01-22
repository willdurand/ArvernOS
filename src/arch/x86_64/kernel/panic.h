/** @file */
#ifndef CORE_PANIC_H
#define CORE_PANIC_H

#include <logging.h>

#define __PANIC(format, ...)                                                   \
  DEBUG(format "%s", __VA_ARGS__);                                             \
  kernel_panic("\nPANIC in %s() (line %d): " format "%s",                      \
               __func__,                                                       \
               __LINE__,                                                       \
               __VA_ARGS__);

/// This macro should only be used when something goes terribly wrong.
#define PANIC(...) __PANIC(__VA_ARGS__, "\n")

typedef struct stack_frame
{
  struct stack_frame* rbp;
  uint64_t rip;
} stack_frame_t;

/**
 * Handles a kernel panic.
 *
 * @param format the reason for the kernel panic
 */
void kernel_panic(const char* format, ...);

void kernel_dump_stacktrace();

#endif
