#ifndef SYS_LOGGING_H
#define SYS_LOGGING_H

#include <logging.h>

#ifdef ENABLE_SYS_DEBUG
#define SYS_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define SYS_DEBUG(format, ...)
#endif

#endif
