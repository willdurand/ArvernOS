#ifndef LOGGING_H
#define LOGGING_H

#include <inttypes.h>
#include <stdio.h>

// See: https://github.com/shiena/ansicolor
#define ANSICOLOR_RESET        "\x1b[0m"
#define ANSICOLOR_FG_CYAN      "\x1b[36m"
#define ANSICOLOR_FG_LIGHTGRAY "\x1b[90m"
#define ANSICOLOR_FG_YELLOW    "\x1b[33m"
#define ANSICOLOR_FG_RED       "\x1b[31m"

#ifdef ENABLE_LOGS_FOR_TESTS

#define LOG(level, level_color, format, ...)                                   \
  printf("%s%s:%" PRIu64 ":%s(): " format "%s\n",                              \
         ANSICOLOR_FG_LIGHTGRAY,                                               \
         __FILE__,                                                             \
         (uint64_t)__LINE__,                                                   \
         __func__,                                                             \
         __VA_ARGS__,                                                          \
         ANSICOLOR_RESET)

#else // ENABLE_LOGS_FOR_TESTS

#ifdef LOGS_WITH_COLORS
// We use colors because the serial port is bound to a file in debug mode (see
// qemu config in `Makefile`). That would not work otherwise, I think.
#define LOG(level, level_color, format, ...)                                   \
  fctprintf(&logging_stream_output,                                            \
            NULL,                                                              \
            "%s%-8s%s | %s%s:%" PRIu64 ":%s():%s " format "\n",                \
            level_color,                                                       \
            level,                                                             \
            ANSICOLOR_RESET,                                                   \
            ANSICOLOR_FG_LIGHTGRAY,                                            \
            __FILE__,                                                          \
            (uint64_t)__LINE__,                                                \
            __func__,                                                          \
            ANSICOLOR_RESET,                                                   \
            __VA_ARGS__)

#else // LOGS_WITH_COLORS

#define LOG(level, level_color, format, ...)                                   \
  fctprintf(&logging_stream_output,                                            \
            NULL,                                                              \
            "%-8s | %s:%" PRIu64 ":%s(): " format "\n",                        \
            level,                                                             \
            __FILE__,                                                          \
            (uint64_t)__LINE__,                                                \
            __func__,                                                          \
            __VA_ARGS__)

#endif // LOGS_WITH_COLORS

#endif // ENABLE_LOGS_FOR_TESTS

#if defined(ENABLE_KERNEL_DEBUG) || defined(ENABLE_LOGS_FOR_TESTS)

#define DEBUG(format, ...) LOG("DEBUG", ANSICOLOR_FG_CYAN, format, __VA_ARGS__)

#define HEX_DEBUG(data, len)                                                   \
  DEBUG("(hexdump) data=%p len=%" PRIu64, data, (uint64_t)len);                \
  for (uint64_t i = 0; i < len; i++) {                                         \
    if (i > 0 && i % 8 == 0) {                                                 \
      fctprintf(&logging_stream_output, NULL, "\n");                           \
    }                                                                          \
    fctprintf(&logging_stream_output, NULL, "%02x ", (uint8_t)(data + i));     \
  }                                                                            \
  fctprintf(&logging_stream_output, NULL, "\n");

#else // ENABLE_KERNEL_DEBUG || ENABLE_LOGS_FOR_TESTS

#define DEBUG(format, ...)                                                     \
  do {                                                                         \
  } while (0)

#define HEX_DEBUG(data, len)                                                   \
  do {                                                                         \
  } while (0)

#endif // ENABLE_KERNEL_DEBUG || ENABLE_LOGS_FOR_TESTS

#define INFO(format, ...) LOG("INFO", ANSICOLOR_FG_YELLOW, format, __VA_ARGS__)

#define ERROR(format, ...) LOG("ERROR", ANSICOLOR_FG_RED, format, __VA_ARGS__)

/**
 * Architecture-specific code should implement this function to output logs
 * somewhere. In most cases, a serial line will be used.
 *
 * @param c a character to print
 * @param arg optional arguments. This should be `NULL` all the time.
 */
void logging_stream_output(char c, void* arg);

#endif
