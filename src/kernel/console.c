#include "console.h"
#include <string.h>

static vtcell_t buffer[CONSOLE_WIDTH * CONSOLE_HEIGHT] = { 0 };

static vtconsole_t vtc = { .width = CONSOLE_WIDTH,
                           .height = CONSOLE_HEIGHT,
                           .ansiparser =
                             (vtansi_parser_t){ VTSTATE_ESC, { { 0, 0 } }, 0 },
                           .attr = VTC_DEFAULT_ATTR,
                           .buffer = buffer,
                           .cursor = (vtcursor_t){ 0, 0 },
                           .on_paint = 0,
                           .on_move = 0 };

void console_init(multiboot_tag_framebuffer_common_t* common)
{
  if (common->framebuffer_addr != 0xB8000) {
    vbe_console_init(common);
    vtc.on_paint = vbe_on_paint_callback;
    vtc.on_move = vbe_on_move_callback;
  } else {
    vga_console_init();
    vtc.on_paint = vga_on_paint_callback;
    vtc.on_move = vga_on_move_callback;
  }

  vtconsole_clear(&vtc, 0, 0, CONSOLE_WIDTH, CONSOLE_HEIGHT - 1);
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
