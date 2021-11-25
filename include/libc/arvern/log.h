#ifndef ARVERN_LOG_H
#define ARVERN_LOG_H

#ifndef __is_libk
#include <sys/types.h>

#define _ARVERN_LOG(format, ...)                                               \
  fctprintf(&arvern_log_putchar,                                               \
            NULL,                                                              \
            "USERLAND: %s:%llu:%s(): " format "%s",                            \
            __FILE__,                                                          \
            (uint64_t)__LINE__,                                                \
            __func__,                                                          \
            __VA_ARGS__)

#define ARVERN_LOG(...) _ARVERN_LOG(__VA_ARGS__, "\n")

void arvern_log_init();

void arvern_log_deinit();

void arvern_log_putchar(char c, void* arg);

#endif // __is_libk

#endif
