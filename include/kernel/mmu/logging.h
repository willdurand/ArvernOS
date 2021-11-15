#ifndef MMU_LOGGING_H
#define MMU_LOGGING_H

#include <logging.h>

#if defined(ENABLE_MMU_DEBUG) || defined(ENABLE_LOGS_FOR_TESTS)
#define MMU_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define MMU_DEBUG(format, ...)                                                 \
  do {                                                                         \
  } while (0)
#endif

#endif
