#include "vga_text.h"
#include <core/port.h>
#include <logging.h>
#include <string.h>

#define FB_COMMAND_PORT      0x3D4
#define FB_DATA_PORT         0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

static uint16_t* framebuffer = (uint16_t*)VIDEO_ADDRESS;

void vga_text_init()
{
  memset(framebuffer, 0, VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * sizeof(uint16_t));
}

void vga_text_write_at(char c, uint8_t scheme, uint8_t x, uint8_t y)
{
  if (x < VGA_TEXT_WIDTH) {
    if (y < VGA_TEXT_WIDTH) {
      framebuffer[y * VGA_TEXT_WIDTH + x] = scheme << 8 | (c & 0xFF);
    }
  }
}

void vga_text_move_cursor(uint8_t x, uint8_t y)
{
  uint16_t pos = y * VGA_TEXT_WIDTH + x;

  port_byte_out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  port_byte_out(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  port_byte_out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  port_byte_out(FB_DATA_PORT, pos & 0x00FF);
}

void vga_text_disable_cursor()
{
  port_byte_out(FB_COMMAND_PORT, 0x0A);
  port_byte_out(FB_DATA_PORT, 0x20);
}
