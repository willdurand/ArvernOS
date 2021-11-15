#ifndef CONFIG_LOGGING_H
#define CONFIG_LOGGING_H

#include <logging.h>

#ifdef ENABLE_CONFIG_DEBUG
#define CONFIG_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define CONFIG_DEBUG(format, ...)                                              \
  do {                                                                         \
  } while (0)
#endif

#endif
