#ifndef MMU_LOGGING_H
#define MMU_LOGGING_H

#include <logging.h>

<<<<<<< HEAD:src/mmu/debug.h
#ifdef DISABLE_MMU_DEBUG
#define MMU_DEBUG(format, ...)
#else
#define MMU_DEBUG(format, ...) DEBUG_OUT(format, __VA_ARGS__)
=======
#ifdef ENABLE_MMU_DEBUG
#define MMU_DEBUG(format, ...) DEBUG(format, __VA_ARGS__)
#else
#define MMU_DEBUG(format, ...)
>>>>>>> cd080736337f92180c8e1821d448c419256c5e74:src/mmu/logging.h
#endif

#endif
