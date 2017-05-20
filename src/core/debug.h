#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <drivers/serial.h>

#define DEBUG(format, ...)  serial_printf(SERIAL_COM1, \
                            "DEBUG: %s:%d:%s(): " format, \
                            __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif
