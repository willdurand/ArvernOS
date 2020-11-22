#include "console.h"
#include <drivers/vga_text.h>
#include <vtconsole/vtconsole.h>

void on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y);
void on_move_callback(vtconsole_t* vtc, vtcursor_t* cur);

vtcell_t buffer[VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT] = { 0 };
vtconsole_t vtc = { .width = VGA_TEXT_WIDTH,
                    .height = VGA_TEXT_HEIGHT,
                    .ansiparser =
                      (vtansi_parser_t){ VTSTATE_ESC, { { 0, 0 } }, 0 },
                    .attr = VTC_DEFAULT_ATTR,
                    .buffer = buffer,
                    .cursor = (vtcursor_t){ 0, 0 },
                    .on_paint = on_paint_callback,
                    .on_move = on_move_callback };

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

void console_init()
{
  vga_text_init();

  vtconsole_clear(&vtc, 0, 0, VGA_TEXT_WIDTH, VGA_TEXT_HEIGHT - 1);
}

void on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y)
{
  if (cell->attr.bright) {
    vga_text_write_at(
      cell->c, brightcolors[cell->attr.fg] | colors[cell->attr.bg] << 4, x, y);
  } else {
    vga_text_write_at(
      cell->c, colors[cell->attr.fg] | colors[cell->attr.bg] << 4, x, y);
  }
}

void on_move_callback(vtconsole_t* vtc, vtcursor_t* cur)
{
  vga_text_move_cursor(cur->x, cur->y);
}

void console_putchar(char c)
{
  vtconsole_putchar(&vtc, c);
}

size_t console_write(const char* s, size_t count)
{
  if (count > strlen(s)) {
    count = strlen(s);
  }

  vtconsole_write(&vtc, s, count);

  return count;
}
