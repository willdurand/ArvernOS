/** @file */
#ifndef ASSERT_H
#define ASSERT_H

#if defined(__is_libk) && defined(DEBUG_MODE)

#include <panic.h>

#define assert(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      PANIC("Assertion failed at %s:%d:%s(): %s",                              \
            __FILE__,                                                          \
            __LINE__,                                                          \
            __func__,                                                          \
            #expr);                                                            \
    }                                                                          \
  } while (0)

#else // __is_libk

#define assert(ignore) ((void)0)

#endif // __is_libk

#define static_assert _Static_assert

#endif
