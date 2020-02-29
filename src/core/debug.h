#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#ifdef ENABLE_KERNEL_DEBUG
#include <drivers/serial.h>
#include <stdio.h>

static const uint16_t serial_com1 = SERIAL_COM1;

#ifdef DEBUG_WITH_COLORS
// See: https://github.com/shiena/ansicolor
#define ANSICOLOR_RESET        "\x1b[0m"
#define ANSICOLOR_FG_CYAN      "\x1b[36m"
#define ANSICOLOR_FG_LIGHTGRAY "\x1b[90m"
#define ANSICOLOR_FG_YELLOW    "\x1b[33m"

// We use colors because the serial port is bound to a file in debug mode (see
// qemu config in `Makefile`). That would not work otherwise, I think.
#define DEBUG(format, ...)                                                     \
  fctprintf(&serial_stream_output,                                             \
            (void*)&serial_com1,                                               \
            "%sDEBUG%s %s%s:%ld:%s():%s " format "\n",                         \
            ANSICOLOR_FG_CYAN,                                                 \
            ANSICOLOR_RESET,                                                   \
            ANSICOLOR_FG_LIGHTGRAY,                                            \
            __FILE__,                                                          \
            __LINE__,                                                          \
            __func__,                                                          \
            ANSICOLOR_RESET,                                                   \
            __VA_ARGS__)
#else

#define DEBUG(format, ...)                                                     \
  fctprintf(&serial_stream_output,                                             \
            (void*)&serial_com1,                                               \
            "DEBUG %s:%ld:%s(): " format "\n",                                 \
            __FILE__,                                                          \
            __LINE__,                                                          \
            __func__,                                                          \
            __VA_ARGS__)

#endif

#else

#ifdef ENABLE_DEBUG_FOR_TEST
#include <stdio.h>

#define DEBUG(format, ...)                                                     \
  printf("DEBUG %s:%d:%s(): " format "\n",                                     \
         __FILE__,                                                             \
         __LINE__,                                                             \
         __func__,                                                             \
         __VA_ARGS__)
#else

#define DEBUG(format, ...)

#endif

#endif

#endif
