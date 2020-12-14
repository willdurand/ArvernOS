#ifndef FS_LOGGING_H
#define FS_LOGGING_H

#include <logging.h>

#ifdef ENABLE_FS_DEBUG
#define FS_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define FS_DEBUG(format, ...)
#endif

#endif
