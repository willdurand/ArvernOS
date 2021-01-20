#include "console.h"
#include <logging.h>
#include <stdlib.h>
#include <string.h>

static multiboot_tag_framebuffer_common_t* fb_common = NULL;

static vtcell_t static_buffer[CONSOLE_WIDTH * CONSOLE_HEIGHT] = { 0 };
static vtconsole_t static_vtc = {
  .width = CONSOLE_WIDTH,
  .height = CONSOLE_HEIGHT,
  .ansiparser = (vtansi_parser_t){ VTSTATE_ESC, { { 0, 0 } }, 0 },
  .attr = VTC_DEFAULT_ATTR,
  .buffer = static_buffer,
  .cursor = (vtcursor_t){ 0, 0 },
  .on_paint = 0,
  .on_move = 0
};
static vtconsole_t* vtc = &static_vtc;

void console_init(multiboot_tag_framebuffer_common_t* common)
{
  fb_common = common;

  if (console_mode_is_vbe()) {
    vbe_console_init(common);
    vtc->on_paint = vbe_on_paint_callback;
    vtc->on_move = vbe_on_move_callback;
  } else {
    vga_console_init();
    vtc->on_paint = vga_on_paint_callback;
    vtc->on_move = vga_on_move_callback;
  }

  vtconsole_clear(vtc, 0, 0, CONSOLE_WIDTH, CONSOLE_HEIGHT - 1);
}

bool console_fullscreen()
{
  if (!console_mode_is_vbe()) {
    return false;
  }

  uint64_t width = fb_common->framebuffer_width / CONSOLE_FONT_WIDTH;
  uint64_t height = fb_common->framebuffer_height / CONSOLE_FONT_HEIGHT;

  vtconsole_t* v = malloc(sizeof(vtconsole_t));
  v->width = width;
  v->height = height;
  v->ansiparser = (vtansi_parser_t){ VTSTATE_ESC, { { 0, 0 } }, 0 };
  v->attr = VTC_DEFAULT_ATTR;
  v->buffer = malloc(width * height * sizeof(vtcell_t));
  v->cursor = (vtcursor_t){ 0, 0 };
  v->on_paint = vtc->on_paint;
  v->on_move = vtc->on_move;
  vtc = v;

  vtconsole_clear(vtc, 0, 0, width, height - 1);

  return true;
}

void console_putchar(char c)
{
  vtconsole_putchar(vtc, c);
}

size_t console_write(const char* s, size_t count)
{
  if (count > strlen(s)) {
    count = strlen(s);
  }

  vtconsole_write(vtc, s, count);

  return count;
}

bool console_mode_is_vbe()
{
  return fb_common->framebuffer_addr != 0xB8000;
}
