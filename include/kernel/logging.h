#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

#define LOG_LEVEL_FATAL 60
#define LOG_LEVEL_ERROR 50
#define LOG_LEVEL_WARN  40
#define LOG_LEVEL_INFO  30
#define LOG_LEVEL_DEBUG 20
#define LOG_LEVEL_TRACE 10

#define CURRENT_SOURCE_LOCATION                                                \
  (source_location_t) { .file = __FILE__, .func = __func__, .line = __LINE__ }

#define FATAL(format, ...)                                                     \
  logging_impl(LOG_LEVEL_FATAL,                                                \
               LOGGING_LEVEL,                                                  \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define ERROR(format, ...)                                                     \
  logging_impl(LOG_LEVEL_ERROR,                                                \
               LOGGING_LEVEL,                                                  \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define WARN(format, ...)                                                      \
  logging_impl(LOG_LEVEL_WARN,                                                 \
               LOGGING_LEVEL,                                                  \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define INFO(format, ...)                                                      \
  logging_impl(LOG_LEVEL_INFO,                                                 \
               LOGGING_LEVEL,                                                  \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define DEBUG(format, ...)                                                     \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL,                                                  \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define TRACE(format, ...)                                                     \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL,                                                  \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

typedef struct source_location
{
  const char* file;
  const char* func;
  uint64_t line;
} source_location_t;

void logging_impl(uint8_t level,
                  uint8_t minimum_level,
                  source_location_t loc,
                  const char* format,
                  ...);

#endif
