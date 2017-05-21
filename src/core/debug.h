#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <drivers/serial.h>

#ifdef ENABLE_KERNEL_DEBUG
#define DEBUG(format, ...)  serial_printf(SERIAL_COM1, \
                            "DEBUG: %s:%d:%s(): " format "\n", \
                            __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

#endif
