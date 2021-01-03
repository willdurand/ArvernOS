#ifndef CORE_LOGGING_H
#define CORE_LOGGING_H

#include <logging.h>

#ifdef ENABLE_CORE_DEBUG
#define CORE_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define CORE_DEBUG(format, ...)                                                \
  do {                                                                         \
  } while (0)
#endif

#endif
