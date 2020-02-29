#include "screen.h"
#include <core/debug.h>
#include <core/port.h>
#include <stddef.h>
#include <string.h>

#define FB_COMMAND_PORT      0x3D4
#define FB_DATA_PORT         0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

// private
void screen_write_at(char c, uint8_t scheme, int x, int y);
uint8_t color_scheme(uint8_t fg, uint8_t bg);
void move_cursor(uint16_t pos);

uint8_t screen_scheme;
char* framebuffer;
int screen_col;
int screen_row;

void screen_init()
{
  framebuffer = (char*)VIDEO_ADDRESS;
  screen_col = 0;
  screen_row = 0;
  screen_color_scheme(COLOR_WHITE, COLOR_BLACK);
}

void screen_color_scheme(uint8_t fg, uint8_t bg)
{
  screen_scheme = color_scheme(fg, bg);
}

void screen_clear()
{
  memset((char*)VIDEO_ADDRESS, 0, 2 * SCREEN_HEIGHT * SCREEN_WIDTH);

  screen_col = 0;
  screen_row = 0;
}

size_t screen_print(const char* str, size_t count)
{
  if (count > strlen(str)) {
    count = strlen(str);
  }

  for (size_t i = 0; i < count; i++) {
    screen_write(str[i]);
  }

  return count;
}

uint8_t color_scheme(uint8_t fg, uint8_t bg)
{
  return fg | bg << 4;
}

void screen_write_at(char c, uint8_t scheme, int x, int y)
{
  int offset = 2 * (y * SCREEN_WIDTH + x);

  framebuffer[offset] = c;
  framebuffer[offset + 1] = scheme;
}

void screen_write(char c)
{
  if (c == '\n') {
    screen_col = 0;
    screen_row++;
  } else if (c == '\b') {
    if (screen_col == 0) {
      screen_col = SCREEN_WIDTH - 1;
      screen_row--;
    } else {
      screen_col--;
    }

    screen_write_at(0x0, screen_scheme, screen_col, screen_row);
  } else if (c == '\t') {
    screen_print("    ", 4);
  } else if (c == '\r') {
    screen_col = 0;
  } else {
    screen_write_at(c, screen_scheme, screen_col, screen_row);
    screen_col++;
  }

  if (screen_col >= SCREEN_WIDTH) {
    screen_col = 0;
    screen_row++;
  }

  if (screen_row >= SCREEN_HEIGHT) {
    memmove((char*)VIDEO_ADDRESS,
            (char*)VIDEO_ADDRESS + (2 * SCREEN_WIDTH),
            2 * (SCREEN_HEIGHT - 1) * SCREEN_WIDTH);

    memset((char*)VIDEO_ADDRESS + ((SCREEN_HEIGHT - 1) * SCREEN_WIDTH * 2),
           0,
           2 * SCREEN_WIDTH);

    screen_row = SCREEN_HEIGHT - 1;
  }

  move_cursor((screen_row * SCREEN_WIDTH) + screen_col);
}

void move_cursor(uint16_t pos)
{
  port_byte_out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  port_byte_out(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  port_byte_out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  port_byte_out(FB_DATA_PORT, pos & 0x00FF);
}
