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

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <time/timer.h>

/// Timestamp prefix (`[seconds.microseconds]`) for the logger. The seconds are
/// right-aligned on 3 characters and the fractional part is zero-padded to 6
/// digits.
#define __LOG_TIMESTAMP_FORMAT "[%3" PRIu64 ".%03" PRIu64 "%03" PRIu64 "] "

/// Writes one log line to the structured logger.
#define __LOG_STREAM(time_us, ctx, level, format, ...)                         \
  fctprintf(&logging_stream_output,                                            \
            &ctx,                                                              \
            __LOG_TIMESTAMP_FORMAT "%-8s | %s:%d:%s(): " format "\n",          \
            (time_us) / 1000000,                                               \
            ((time_us) / 1000) % 1000,                                         \
            (time_us) % 1000,                                                  \
            level,                                                             \
            __FILE__,                                                          \
            __LINE__,                                                          \
            __func__,                                                          \
            ##__VA_ARGS__)

/// Writes one log line, using the same timestamp for the whole message.
#define __LOG(_level, prefix, format, ...)                                     \
  do {                                                                         \
    uint64_t __time_us = timer_uptime_microseconds();                          \
    log_context_t ctx = { .level = _level };                                   \
    __LOG_STREAM(__time_us, ctx, prefix, format, ##__VA_ARGS__);               \
  } while (0)

#define INFO(format, ...) __LOG(LEVEL_INFO, "INFO", format, ##__VA_ARGS__)
#define WARN(format, ...) __LOG(LEVEL_WARN, "WARN", format, ##__VA_ARGS__)

typedef enum
{
  LEVEL_DEBUG = 0,
  LEVEL_INFO = 1,
  LEVEL_WARN = 2,
  LEVEL_ERROR = 3,
} log_level_t;

typedef struct
{
  log_level_t level;
} log_context_t;

extern bool kernel_boot_complete;

/**
 * This function is used to output logs.
 *
 * @param c a character to print
 * @param arg optional argument, either a `log_context_t` or `NULL`.
 */
void logging_stream_output(char c, void* arg);

#ifdef ENABLE_KERNEL_DEBUG

#define DEBUG(format, ...) __LOG(LEVEL_DEBUG, "DEBUG", format, ##__VA_ARGS__)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
    const uint8_t* __hex_data = (const uint8_t*)(data);                        \
    uint64_t __hex_len = (uint64_t)(len);                                      \
    DEBUG(                                                                     \
      "(hexdump) data=%p len=%" PRIu64, (const void*)__hex_data, __hex_len);   \
    for (uint64_t i = 0; i < __hex_len; i++) {                                 \
      if (i > 0 && i % 8 == 0) {                                               \
        fctprintf(&logging_stream_output, NULL, "\n");                         \
      }                                                                        \
      fctprintf(&logging_stream_output, NULL, "%02x ", __hex_data[i]);         \
    }                                                                          \
    fctprintf(&logging_stream_output, NULL, "\n");                             \
  } while (0)

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
