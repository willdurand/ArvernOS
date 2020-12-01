#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <vtconsole/vtconsole.h>

#include "video_console.h"

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
