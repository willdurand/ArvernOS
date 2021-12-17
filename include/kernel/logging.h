/**
 * @file
 * @brief This is the general kernel logging module.
 *
 * It supports several logging levels such as INFO, DEBUG, and TRACE:
 *
 * - DEBUG logs are not enabled by default. They can be enabled by setting
 *   `ENABLE_KERNEL_DEBUG=0` in config.
 * - TRACE logs are not enabled by default. They can be enabled by setting
 *   `ENABLE_KERNEL_TRACE=1` in config.
 *
 * Each kernel module should ideally have its own logging header that depends
 * on this one. The main use case is to have more configurable loggers that can
 * be enabled/disabled depending on the needs.
 */
#ifndef LOGGING_H
#define LOGGING_H

#ifdef TEST_ENV

#define TRACE(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define DEBUG(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
  } while (0)

#define WARN(format, ...)                                                      \
  do {                                                                         \
  } while (0)

#define INFO(format, ...)                                                      \
  do {                                                                         \
  } while (0)

#else // TEST_ENV

#include <arch/logging.h>
#include <inttypes.h>
#include <stdio.h>
#include <time/timer.h>

#define LOG(level, format, ...)                                                \
  fctprintf(&arch_logging_stream_output,                                       \
            NULL,                                                              \
            "%-8s | %s:%d:%s(): " format "\n",                                 \
            level,                                                             \
            __FILE__,                                                          \
            __LINE__,                                                          \
            __func__,                                                          \
            __VA_ARGS__)

#ifdef ENABLE_KERNEL_DEBUG

#define DEBUG(format, ...) LOG("DEBUG", format, __VA_ARGS__)

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

#define LOG_WARN(format, ...) LOG("WARN", format, __VA_ARGS__)

#define LOG_INFO(format, ...) LOG("INFO", format, __VA_ARGS__)

#else // ENABLE_KERNEL_DEBUG

#define DEBUG(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
  } while (0)

#define LOG_WARN(format, ...)                                                  \
  do {                                                                         \
  } while (0)

#define LOG_INFO(format, ...)                                                  \
  do {                                                                         \
  } while (0)

#endif // ENABLE_KERNEL_DEBUG

#define WARN(format, ...)                                                      \
  LOG_WARN(format, __VA_ARGS__);                                               \
  {                                                                            \
    uint64_t time_us = timer_uptime_microseconds();                            \
    printf("[%3" PRIu64 ".%03" PRIu64 "%03" PRIu64 "] " format "\n",           \
           time_us / 1000000,                                                  \
           time_us / 1000,                                                     \
           time_us % 1000,                                                     \
           __VA_ARGS__);                                                       \
  }

#define INFO(format, ...)                                                      \
  LOG_INFO(format, __VA_ARGS__);                                               \
  {                                                                            \
    uint64_t time_us = timer_uptime_microseconds();                            \
    printf("[%3" PRIu64 ".%03" PRIu64 "%03" PRIu64 "] " format "\n",           \
           time_us / 1000000,                                                  \
           time_us / 1000,                                                     \
           time_us % 1000,                                                     \
           __VA_ARGS__);                                                       \
  }

#ifdef ENABLE_KERNEL_TRACE

#define TRACE(format, ...) LOG("TRACE", format, __VA_ARGS__)

#else // ENABLE_KERNEL_TRACE

#define TRACE(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#endif // ENABLE_KERNEL_TRACE

#endif // TEST_ENV

#endif
