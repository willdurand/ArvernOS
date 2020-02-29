/**
 * @file
 * @see https://en.wikipedia.org/wiki/VGA-compatible_text_mode
 */
#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <sys/types.h>

/// The address of the frame buffer.
#define VIDEO_ADDRESS 0xB8000
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

static const uint8_t COLOR_BLACK = 0;
static const uint8_t COLOR_BLUE = 1;
static const uint8_t COLOR_GREEN = 2;
static const uint8_t COLOR_CYAN = 3;
static const uint8_t COLOR_RED = 4;
static const uint8_t COLOR_MAGENTA = 5;
static const uint8_t COLOR_BROWN = 6;
static const uint8_t COLOR_LIGHT_GREY = 7;
static const uint8_t COLOR_DARK_GREY = 8;
static const uint8_t COLOR_LIGHT_BLUE = 9;
static const uint8_t COLOR_LIGHT_GREEN = 10;
static const uint8_t COLOR_LIGHT_CYAN = 11;
static const uint8_t COLOR_LIGHT_RED = 12;
static const uint8_t COLOR_LIGHT_MAGENTA = 13;
static const uint8_t COLOR_LIGHT_BROWN = 14;
static const uint8_t COLOR_WHITE = 15;

/**
 * Initializes the screen driver.
 */
void screen_init();

/**
 * Clears the entire screen.
 */
void screen_clear();

/**
 * Writes a character to the screen.
 *
 * @param c the character to write to the screen
 */
void screen_write(char c);

/**
 * Prints a string to the screen.
 *
 * @param str the string to print to the screen
 * @param count the number of characters to write
 * @return the numberof characters printed to the screen
 */
size_t screen_print(const char* str, size_t count);

/**
 * Changes the color scheme of the screen (background and foreground) that will
 * be used for subsequent characters printed to it.
 *
 * @param fg the foreground color
 * @param bg the background color
 */
void screen_color_scheme(uint8_t fg, uint8_t bg);

#endif
