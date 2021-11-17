#ifndef LOGGING_H
#define LOGGING_H

#include <inttypes.h>
#include <stdio.h>

#ifdef ENABLE_LOGS_FOR_TESTS

// See: https://github.com/shiena/ansicolor
#define ANSICOLOR_FG_LIGHTGRAY "\x1b[90m"
#define ANSICOLOR_RESET        "\x1b[0m"

#define LOG(level, format, ...)                                                \
  printf("%s%s:%" PRIu64 ":%s(): " format "%s\n",                              \
         ANSICOLOR_FG_LIGHTGRAY,                                               \
         __FILE__,                                                             \
         (uint64_t)__LINE__,                                                   \
         __func__,                                                             \
         __VA_ARGS__,                                                          \
         ANSICOLOR_RESET)

#else // ENABLE_LOGS_FOR_TESTS

#include <arch/logging.h>

#define LOG(level, format, ...)                                                \
  fctprintf(&arch_logging_stream_output,                                       \
            NULL,                                                              \
            "%-8s | %s:%" PRIu64 ":%s(): " format "\n",                        \
            level,                                                             \
            __FILE__,                                                          \
            (uint64_t)__LINE__,                                                \
            __func__,                                                          \
            __VA_ARGS__)

#endif // ENABLE_LOGS_FOR_TESTS

#if defined(ENABLE_KERNEL_DEBUG) || defined(ENABLE_LOGS_FOR_TESTS)

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

#else // ENABLE_KERNEL_DEBUG || ENABLE_LOGS_FOR_TESTS

#define DEBUG(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
  } while (0)

#endif // ENABLE_KERNEL_DEBUG || ENABLE_LOGS_FOR_TESTS

#define INFO(format, ...) LOG("INFO", format, __VA_ARGS__)

#define ERROR(format, ...) LOG("ERROR", format, __VA_ARGS__)

#endif
