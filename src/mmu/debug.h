#ifndef MMU_DEBUG_H
#define MMU_DEBUG_H

#include <logging.h>

#ifdef ENABLE_MMU_DEBUG
#define MMU_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define MMU_DEBUG(format, ...)
#endif

#endif
