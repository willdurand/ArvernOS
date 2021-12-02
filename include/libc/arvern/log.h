/** @file */
#ifndef ARVERN_LOG_H
#define ARVERN_LOG_H

#ifndef __is_libk
#include <sys/types.h>

#ifdef ENABLE_USERLAND_DEBUG

#define LEVEL "USERLAND"
#define _ARVERN_LOG(format, ...)                                               \
  fctprintf(&arvern_log_putchar,                                               \
            NULL,                                                              \
            "%-8s | %s:%llu:%s(): " format "%s",                               \
            LEVEL,                                                             \
            __FILE__,                                                          \
            (uint64_t)__LINE__,                                                \
            __func__,                                                          \
            __VA_ARGS__)

#define ARVERN_LOG(...) _ARVERN_LOG(__VA_ARGS__, "\n")

#else // ENABLE_USERLAND_DEBUG

#define ARVERN_LOG(...)                                                        \
  do {                                                                         \
  } while (0)

#endif // ENABLE_USERLAND_DEBUG

void arvern_log_init();

void arvern_log_deinit();

void arvern_log_putchar(char c, void* arg);

#endif // __is_libk

#endif
