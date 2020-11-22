#include "vga_text.h"
#include <core/debug.h>
#include <core/port.h>
#include <stddef.h>
#include <string.h>

#define FB_COMMAND_PORT      0x3D4
#define FB_DATA_PORT         0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

// private
void vga_text_write_at(char c, uint8_t scheme, int x, int y);
uint8_t color_scheme(uint8_t fg, uint8_t bg);
void move_cursor(uint16_t pos);

uint8_t vga_text_scheme;
char* framebuffer = (char*)VIDEO_ADDRESS;
int vga_text_col = 0;
int vga_text_row = 0;

void vga_text_init()
{
  vga_text_color_scheme(COLOR_WHITE, COLOR_BLACK);
}

void vga_text_color_scheme(uint8_t fg, uint8_t bg)
{
  vga_text_scheme = color_scheme(fg, bg);
}

void vga_text_clear()
{
  memset((char*)VIDEO_ADDRESS, 0, 2 * VGA_TEXT_HEIGHT * VGA_TEXT_WIDTH);

  vga_text_col = 0;
  vga_text_row = 0;
}

size_t vga_text_print(const char* str, size_t count)
{
  if (count > strlen(str)) {
    count = strlen(str);
  }

  for (size_t i = 0; i < count; i++) {
    vga_text_write(str[i]);
  }

  return count;
}

uint8_t color_scheme(uint8_t fg, uint8_t bg)
{
  return fg | bg << 4;
}

void vga_text_write_at(char c, uint8_t scheme, int x, int y)
{
  int offset = 2 * (y * VGA_TEXT_WIDTH + x);

  framebuffer[offset] = c;
  framebuffer[offset + 1] = scheme;
}

void vga_text_write(char c)
{
  if (c == '\n') {
    vga_text_col = 0;
    vga_text_row++;
  } else if (c == '\b') {
    if (vga_text_col == 0) {
      vga_text_col = VGA_TEXT_WIDTH - 1;
      vga_text_row--;
    } else {
      vga_text_col--;
    }

    vga_text_write_at(0x0, vga_text_scheme, vga_text_col, vga_text_row);
  } else if (c == '\t') {
    vga_text_print("    ", 4);
  } else if (c == '\r') {
    vga_text_col = 0;
  } else {
    vga_text_write_at(c, vga_text_scheme, vga_text_col, vga_text_row);
    vga_text_col++;
  }

  if (vga_text_col >= VGA_TEXT_WIDTH) {
    vga_text_col = 0;
    vga_text_row++;
  }

  if (vga_text_row >= VGA_TEXT_HEIGHT) {
    memmove((char*)VIDEO_ADDRESS,
            (char*)VIDEO_ADDRESS + (2 * VGA_TEXT_WIDTH),
            2 * (VGA_TEXT_HEIGHT - 1) * VGA_TEXT_WIDTH);

    memset((char*)VIDEO_ADDRESS + ((VGA_TEXT_HEIGHT - 1) * VGA_TEXT_WIDTH * 2),
           0,
           2 * VGA_TEXT_WIDTH);

    vga_text_row = VGA_TEXT_HEIGHT - 1;
  }

  move_cursor((vga_text_row * VGA_TEXT_WIDTH) + vga_text_col);
}

void move_cursor(uint16_t pos)
{
  port_byte_out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  port_byte_out(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  port_byte_out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  port_byte_out(FB_DATA_PORT, pos & 0x00FF);
}
