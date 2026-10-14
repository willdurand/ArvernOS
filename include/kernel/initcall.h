/**
 * @file
 *
 * Initcall functions are automatically called when the kernel starts, after
 * the architecture-specific code has booted it.
 *
 * Use the `initcall_*_register()` helper functions to register them.
 */
#ifndef INITCALL_H
#define INITCALL_H

#include <stdint.h>

#define INITCALL_CORE 1
#define INITCALL_FS   2
#define INITCALL_LATE 3

#define INITCALL_PRIORITY_EARLIEST 1
#define INITCALL_PRIORITY_LATEST   5

/// This type defines an initcall function, which is a function automatically
/// called when the kernel is started. Use `initcall_*_register()` helper
/// functions to register them.
typedef int (*initcall_t)();

extern unsigned char __initcall_start;
extern unsigned char __initcall_end;

/// Call the registered initcall functions, in ascending level and priority
/// order, and panics if any of them returns a non-zero value. Called once by
/// `kmain_start()`.
void initcall_init();

// We must use `retain` in addition to `used` because of what's described in
// https://lists.llvm.org/pipermail/llvm-dev/2021-February/148760.html and
// https://reviews.llvm.org/D97447.
#define __initcall(func, level, priority)                                      \
  static initcall_t __attribute__((                                            \
    used, retain, __section__(".initcall_fn_" #level #priority)))              \
  __initcall_##level##_##priority##_##func = func

#define _initcall(func, level, priority) __initcall(func, level, priority)

#define initcall_core_register_with_priority(func, priority)                   \
  _initcall(func, INITCALL_CORE, priority)

#define initcall_core_register(func)                                           \
  initcall_core_register_with_priority(func, INITCALL_PRIORITY_LATEST)

#define initcall_fs_register_with_priority(func, priority)                     \
  _initcall(func, INITCALL_FS, priority)

#define initcall_fs_register(func)                                             \
  initcall_fs_register_with_priority(func, INITCALL_PRIORITY_LATEST)

#define initcall_late_register_with_priority(func, priority)                   \
  _initcall(func, INITCALL_LATE, priority)

#define initcall_late_register(func)                                           \
  initcall_late_register_with_priority(func, INITCALL_PRIORITY_LATEST)

#define initcall_foreach(itr)                                                  \
  for (initcall_t* itr = (initcall_t*)&__initcall_start;                       \
       itr < (initcall_t*)&__initcall_end;                                     \
       itr++)

#endif
