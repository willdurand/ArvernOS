#ifndef MMU_DEBUG_H
#define MMU_DEBUG_H

#include <core/debug.h>

#ifdef DISABLE_MMU_DEBUG
#define MMU_DEBUG(format, ...)
#else
#define MMU_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#endif

#endif
