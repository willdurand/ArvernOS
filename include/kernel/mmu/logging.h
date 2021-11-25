#ifndef MMU_LOGGING_H
#define MMU_LOGGING_H

#include <logging.h>

#define MMU_DEBUG(format, ...)                                                 \
  logging_impl(LOG_LEVEL_DEBUG,                                                \
               LOGGING_LEVEL_MMU,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#define MMU_TRACE(format, ...)                                                 \
  logging_impl(LOG_LEVEL_TRACE,                                                \
               LOGGING_LEVEL_MMU,                                              \
               CURRENT_SOURCE_LOCATION,                                        \
               format,                                                         \
               __VA_ARGS__)

#endif
