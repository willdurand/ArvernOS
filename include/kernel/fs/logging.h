#ifndef FS_LOGGING_H
#define FS_LOGGING_H

#include <logging.h>

#define FS_DEBUG(format, ...)                                                  \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_FS,                                               \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define FS_TRACE(format, ...)                                                  \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_FS,                                               \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
