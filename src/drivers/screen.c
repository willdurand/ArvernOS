#include "screen.h"
#include <mem.h>
#include <string.h>
#include <stddef.h>
#include <core/ports.h>

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

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
    framebuffer = (char*) VIDEO_ADDRESS;
    screen_scheme = color_scheme(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_col = 0;
    screen_row = 0;
}

void screen_clear()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            screen_write_at(' ', screen_scheme, x, y);
        }
    }
}

void screen_print(const char* str)
{
    for (size_t i = 0; i < strlen(str); i++) {
        screen_write(str[i]);
    }
}

uint8_t color_scheme(uint8_t fg, uint8_t bg)
{
    return fg | bg << 4;
}

void screen_write_at(char c, uint8_t scheme, int x, int y)
{
    const int offset = 2 * (y * SCREEN_WIDTH + x);

    framebuffer[offset] = c;
    framebuffer[offset + 1] = scheme;

    // scrolling
    if (offset > SCREEN_HEIGHT * SCREEN_WIDTH * 2) {
        for (int i = 1; i < SCREEN_HEIGHT; i++) {
            memcpy(
                ((char *) VIDEO_ADDRESS + (2 * i * SCREEN_WIDTH)),
                ((char *) VIDEO_ADDRESS + (2 * (i - 1) * SCREEN_WIDTH)),
                2 * SCREEN_WIDTH
            );
        }

        char* last_line = (char *)(2 * (SCREEN_HEIGHT) * SCREEN_WIDTH + VIDEO_ADDRESS);
        for (int i = 0; i < SCREEN_WIDTH * 2; i++) {
            last_line[i] = 0;
        }

        screen_row--;
    }
}

void screen_write(char c)
{
    if (c == '\n') {
        screen_col = 0;
        screen_row++;
    } else if (c == '\b' && screen_col > 0) {
        screen_col--;
        screen_write_at(0x0, screen_scheme, screen_col, screen_row);
    } else if (c == '\t') {
        screen_col = screen_col + 8 - (screen_col % 8);
    } else if (c == '\r') {
        screen_col = 0;
    } else {
        screen_write_at(c, screen_scheme, screen_col, screen_row);
        screen_col++;

        if (screen_col == SCREEN_WIDTH) {
            screen_col = 0;
            screen_row++;
        }
    }

    move_cursor((screen_row * SCREEN_WIDTH) + screen_col);
}

void move_cursor(uint16_t pos)
{
    port_byte_out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    port_byte_out(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    port_byte_out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    port_byte_out(FB_DATA_PORT,    pos & 0x00FF);
}
