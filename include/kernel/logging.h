/**
 * @file
 * @brief This is the general kernel logging module.
 *
 * In release mode, the logging levels available are:
 *
 * - INFO
 * - WARN
 *
 * In debug mode, the following additional logging levels are available:
 *
 * - DEBUG: enabled by default but can be disabled with `ENABLE_KERNEL_DEBUG`
 *   set to `0`.
 *
 * Each kernel module should ideally have its own logging header that depends
 * on this one. The main use case is to have more configurable loggers that can
 * be enabled/disabled depending on the needs.
 */
#ifndef LOGGING_H
#define LOGGING_H

#ifdef TEST_ENV

#define INFO(format, ...)                                                      \
  do {                                                                         \
  } while (0)

#define WARN(format, ...)                                                      \
  do {                                                                         \
  } while (0)

#define DEBUG(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
  } while (0)

#else // TEST_ENV

#include <arch/logging.h>
#include <inttypes.h>
#include <stdio.h>
#include <time/timer.h>

#define __LOG(level, format, ...)                                              \
  fctprintf(&arch_logging_stream_output,                                       \
            NULL,                                                              \
            "%-8s | %s:%d:%s(): " format "\n",                                 \
            level,                                                             \
            __FILE__,                                                          \
            __LINE__,                                                          \
            __func__,                                                          \
            __VA_ARGS__)

#define __LOG_INFO(format, ...) __LOG("INFO", format, __VA_ARGS__)
#define INFO(format, ...)                                                      \
  __LOG_INFO(format, __VA_ARGS__);                                             \
  {                                                                            \
    uint64_t time_us = timer_uptime_microseconds();                            \
    printf("[%3" PRIu64 ".%03" PRIu64 "%03" PRIu64 "] " format "\n",           \
           time_us / 1000000,                                                  \
           time_us / 1000,                                                     \
           time_us % 1000,                                                     \
           __VA_ARGS__);                                                       \
  }

#define __LOG_WARN(format, ...) __LOG("WARN", format, __VA_ARGS__)
#define WARN(format, ...)                                                      \
  __LOG_WARN(format, __VA_ARGS__);                                             \
  {                                                                            \
    uint64_t time_us = timer_uptime_microseconds();                            \
    printf("[%3" PRIu64 ".%03" PRIu64 "%03" PRIu64 "] " format "\n",           \
           time_us / 1000000,                                                  \
           time_us / 1000,                                                     \
           time_us % 1000,                                                     \
           __VA_ARGS__);                                                       \
  }

#ifdef ENABLE_KERNEL_DEBUG

#define DEBUG(format, ...) __LOG("DEBUG", format, __VA_ARGS__)

#define HEX_DEBUG(data, len)                                                   \
  DEBUG("(hexdump) data=%p len=%" PRIu64, data, (uint64_t)len);                \
  for (uint64_t i = 0; i < len; i++) {                                         \
    if (i > 0 && i % 8 == 0) {                                                 \
      fctprintf(&arch_logging_stream_output, NULL, "\n");                      \
    }                                                                          \
    fctprintf(                                                                 \
      &arch_logging_stream_output, NULL, "%02x ", (uint8_t)(data + i));        \
  }                                                                            \
  fctprintf(&arch_logging_stream_output, NULL, "\n");

#else // ENABLE_KERNEL_DEBUG

#define DEBUG(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
  } while (0)

#endif // ENABLE_KERNEL_DEBUG

#endif // TEST_ENV

#endif
