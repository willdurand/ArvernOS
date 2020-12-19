#ifndef FS_LOGGING_H
#define FS_LOGGING_H

#include <logging.h>

#if defined(ENABLE_FS_DEBUG) || defined(ENABLE_LOGS_FOR_TESTS)
#define FS_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define FS_DEBUG(format, ...)
#endif

#endif
