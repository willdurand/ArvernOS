#ifndef NET_LOGGING_H
#define NET_LOGGING_H

#include <logging.h>

#define NET_INFO(format, ...)                                                  \
  logging_impl(LOG_LEVEL_INFO,                                                 \
               LOGGING_LEVEL_NET,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define NET_DEBUG(format, ...)                                                 \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_NET,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define NET_TRACE(format, ...)                                                 \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_NET,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
