#ifndef PANIC_H
#define PANIC_H

#include <arch/panic.h>
#include <logging.h>
#include <stdbool.h>

#define __PANIC(dump_stacktrace, format, ...)                                  \
  DEBUG(format "%s", __VA_ARGS__);                                             \
  arch_kernel_panic(dump_stacktrace,                                           \
                    "PANIC in %s() (line %d): " format "%s",                   \
                    __func__,                                                  \
                    __LINE__,                                                  \
                    __VA_ARGS__);

/// This macro should only be used when something goes terribly wrong.
#define PANIC(...) __PANIC(true, __VA_ARGS__, "\n")

#define PANIC_NO_STACKTRACE(...) __PANIC(false, __VA_ARGS__, "\n")

#endif
