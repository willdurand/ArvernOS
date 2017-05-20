#include <string.h>
#include <mem.h>
#include <core/ports.h>
#include "screen.h"

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

// private
void print_char_at(char c, uint8_t scheme, int x, int y);
void print_char(char c);
int strlen(const char* str);
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
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            print_char_at(' ', screen_scheme, x, y);
        }
    }
}

void screen_print(const char* str)
{
    for (int i = 0; i < strlen(str); i++) {
        print_char(str[i]);
    }
}
uint8_t color_scheme(uint8_t fg, uint8_t bg)
{
    return fg | bg << 4;
}

void print_char_at(char c, uint8_t scheme, int x, int y)
{
    const int offset = 2 * (y * VGA_WIDTH + x);

    framebuffer[offset] = c;
    framebuffer[offset + 1] = scheme;

    // scrolling
    if (offset > VGA_HEIGHT * VGA_WIDTH * 2) {
        for (int i = 1; i < VGA_HEIGHT; i++) {
            memcpy(
                ((char *) VIDEO_ADDRESS + (2 * i * VGA_WIDTH)),
                ((char *) VIDEO_ADDRESS + (2 * (i - 1) * VGA_WIDTH)),
                2 * VGA_WIDTH
            );
        }

        char* last_line = (char *)(2 * (VGA_HEIGHT) * VGA_WIDTH + VIDEO_ADDRESS);
        for (int i = 0; i < VGA_WIDTH * 2; i++) {
            last_line[i] = 0;
        }

        screen_row--;
    }
}

void print_char(char c)
{
    if (c == '\n') {
        screen_col = 0;
        screen_row++;
    } else if (c == '\b' && screen_col > 0) {
        screen_col--;
        print_char_at(0x0, screen_scheme, screen_col, screen_row);
    } else if (c == '\t') {
        screen_col = screen_col + 8 - (screen_col % 8);
    } else if (c == '\r') {
        screen_col = 0;
    } else {
        print_char_at(c, screen_scheme, screen_col, screen_row);
        screen_col++;

        if (screen_col == VGA_WIDTH) {
            screen_col = 0;
            screen_row++;
        }
    }

    move_cursor((screen_row * VGA_WIDTH) + screen_col);
}

void move_cursor(uint16_t pos)
{
    port_byte_out(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    port_byte_out(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    port_byte_out(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    port_byte_out(FB_DATA_PORT,    pos & 0x00FF);
}
