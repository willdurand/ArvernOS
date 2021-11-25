#ifndef PROC_LOGGING_H
#define PROC_LOGGING_H

#include <logging.h>

#define PROC_DEBUG(format, ...)                                                \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_PROC,                                             \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define PROC_TRACE(format, ...)                                                \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_PROC,                                             \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
