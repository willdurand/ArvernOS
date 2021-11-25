#ifndef CORE_LOGGING_H
#define CORE_LOGGING_H

#include <logging.h>

#define CORE_DEBUG(format, ...)                                                \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_CORE,                                             \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define CORE_TRACE(format, ...)                                                \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_CORE,                                             \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
