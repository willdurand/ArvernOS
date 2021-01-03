#ifndef WILLOS_LOG_H
#define WILLOS_LOG_H

#ifndef __is_libk
#include <sys/types.h>

#define LEVEL "USERLAND"
#define _WILLOS_LOG(format, ...)                                               \
  fctprintf(&willos_log_putchar,                                               \
            NULL,                                                              \
            "%-8s | %s:%llu:%s(): " format "%s",                               \
            LEVEL,                                                             \
            __FILE__,                                                          \
            (uint64_t)__LINE__,                                                \
            __func__,                                                          \
            __VA_ARGS__)

#define WILLOS_LOG(...) _WILLOS_LOG(__VA_ARGS__, "\n")

void willos_log_init();

void willos_log_deinit();

void willos_log_putchar(char c, void* arg);

#endif

#endif
