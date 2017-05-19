#include "screen.h"

// private
void print_char_at(char c, uint8_t scheme, int x, int y);
void print_char(char c);
int strlen(const char* str);
uint8_t color_scheme(uint8_t fg, uint8_t bg);

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

int strlen(const char* str)
{
    int ret = 0;
    while (str[ret] != 0)
        ret++;

    return ret;
}

void print_char_at(char c, uint8_t scheme, int x, int y)
{
    const int offset = 2 * (y * VGA_WIDTH + x);

    framebuffer[offset] = c;
    framebuffer[offset + 1] = scheme;
}

void print_char(char c)
{
    print_char_at(c, screen_scheme, screen_col, screen_row);

    if (++screen_col == VGA_WIDTH)
    {
        screen_col = 0;
        if (++screen_row == VGA_HEIGHT)
        {
            screen_row = 0;
        }
    }
}
