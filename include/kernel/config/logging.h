#ifndef CONFIG_LOGGING_H
#define CONFIG_LOGGING_H

#include <logging.h>

#define CONFIG_DEBUG(format, ...)                                              \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_CONFIG,                                           \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define CONFIG_TRACE(format, ...)                                              \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_CONFIG,                                           \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
