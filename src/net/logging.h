#ifndef NET_LOGGING_H
#define NET_LOGGING_H

#include <logging.h>

#ifdef ENABLE_NET_DEBUG
#define NET_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define NET_DEBUG(format, ...)
#endif

#endif
