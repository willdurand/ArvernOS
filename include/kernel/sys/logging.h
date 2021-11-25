#ifndef SYS_LOGGING_H
#define SYS_LOGGING_H

#include <logging.h>

#define SYS_DEBUG(format, ...)                                                 \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_SYS,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define SYS_TRACE(format, ...)                                                 \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_SYS,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
