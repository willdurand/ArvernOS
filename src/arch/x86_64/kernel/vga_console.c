#include "console.h"
#include <drivers/vga_text.h>

static char colors[] = {
  [VTCOLOR_BLACK] = COLOR_BLACK, [VTCOLOR_RED] = COLOR_RED,
  [VTCOLOR_GREEN] = COLOR_GREEN, [VTCOLOR_YELLOW] = COLOR_BROWN,
  [VTCOLOR_BLUE] = COLOR_BLUE,   [VTCOLOR_MAGENTA] = COLOR_MAGENTA,
  [VTCOLOR_CYAN] = COLOR_CYAN,   [VTCOLOR_GREY] = COLOR_LIGHT_GREY,
};

static char brightcolors[] = {
  [VTCOLOR_BLACK] = COLOR_DARK_GREY,   [VTCOLOR_RED] = COLOR_LIGHT_RED,
  [VTCOLOR_GREEN] = COLOR_LIGHT_GREEN, [VTCOLOR_YELLOW] = COLOR_LIGHT_YELLOW,
  [VTCOLOR_BLUE] = COLOR_LIGHT_BLUE,   [VTCOLOR_MAGENTA] = COLOR_LIGHT_MAGENTA,
  [VTCOLOR_CYAN] = COLOR_LIGHT_CYAN,   [VTCOLOR_GREY] = COLOR_WHITE,
};

void vga_console_init()
{
  vga_text_init();
}

void vga_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y)
{
  if (cell->attr.bright) {
    vga_text_write_at(
      cell->c, brightcolors[cell->attr.fg] | colors[cell->attr.bg] << 4, x, y);
  } else {
    vga_text_write_at(
      cell->c, colors[cell->attr.fg] | colors[cell->attr.bg] << 4, x, y);
  }
}

void vga_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur)
{
  vga_text_move_cursor(cur->x, cur->y);
}
