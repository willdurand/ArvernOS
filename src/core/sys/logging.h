#ifndef CORE_SYS_LOGGING_H
#define CORE_SYS_LOGGING_H

#include <logging.h>

#ifdef ENABLE_CORE_SYS_DEBUG
#define CORE_SYS_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define CORE_SYS_DEBUG(format, ...)
#endif

#endif
