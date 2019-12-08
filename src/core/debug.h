#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <drivers/serial.h>
#include <stdio.h>

#ifdef ENABLE_KERNEL_DEBUG

static const int serial_com1 = SERIAL_COM1;

#define DEBUG(format, ...)  fctprintf(&serial_stream_output, \
                            (void*)&serial_com1, \
                            "DEBUG %s:%d:%s(): " format "\n", \
                            __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

#endif
