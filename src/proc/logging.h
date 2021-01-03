#ifndef PROC_LOGGING_H
#define PROC_LOGGING_H

#include <logging.h>

#ifdef ENABLE_PROC_DEBUG
#define PROC_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define PROC_DEBUG(format, ...)
#endif

#endif
