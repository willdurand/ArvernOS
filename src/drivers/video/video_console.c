#include <drivers/serial.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vtconsole/vtconsole.h>

#include "video_console.h"

extern vtconsole_t vtc;

static const uint16_t serial_com1 = SERIAL_COM1;

int video_console_x_cursor = 0;
int video_console_y_cursor = 0;
char video_console_print_buffer[10240];

uint32_t video_fps = 0;

uint32_t video_console_colors[] = {
  [VTCOLOR_BLACK] = 0x00000000, [VTCOLOR_RED] = 0xFFFF0000,
  [VTCOLOR_GREEN] = 0xFF008000, [VTCOLOR_YELLOW] = 0xFFA52A2A,
  [VTCOLOR_BLUE] = 0xFF0000FF,  [VTCOLOR_MAGENTA] = 0xFFFF00FF,
  [VTCOLOR_CYAN] = 0xFF00FFFF,  [VTCOLOR_GREY] = 0xFFD3D3D3,
};

uint32_t video_console_bright_colors[] = {
  [VTCOLOR_BLACK] = 0xFFA9A9A9, [VTCOLOR_RED] = 0xFFFFCCCB,
  [VTCOLOR_GREEN] = 0xFF90EE90, [VTCOLOR_YELLOW] = 0xFFFFFF99,
  [VTCOLOR_BLUE] = 0xFFADD8E6,  [VTCOLOR_MAGENTA] = 0xFFE78BE7,
  [VTCOLOR_CYAN] = 0xFFE0FFFF,  [VTCOLOR_GREY] = 0xFFFFFF,
};

void video_console_move_cursor(int x, int y)
{
#if ENABLE_KERNEL_DEBUG
  bool should_print = false;
  int y_index = y;

  if (y != video_console_y_cursor && y > 0) {
    y_index = y - 1;
    should_print = true;
  } else if (x < video_console_x_cursor && y > 0) {
    y_index = y - 1;
    should_print = true;
  }

  if (should_print) {
    vtcell_t* buffer = &vtc.buffer[y_index * vtc.width];

    for (uint32_t i = 0; i < vtc.width; i++) {
      video_console_print_buffer[i] = buffer[i].c;
    }

    video_console_print_buffer[vtc.width] = '\0';

    fctprintf(&serial_stream_output,
              (void*)&serial_com1,
              "%s\n",
              video_console_print_buffer);
  }

  video_console_y_cursor = y;
  video_console_x_cursor = x;
#endif
}
