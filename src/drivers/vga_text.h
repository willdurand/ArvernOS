/**
 * @file
 * @see https://en.wikipedia.org/wiki/VGA-compatible_text_mode
 */
#ifndef VGA_TEXT_H
#define VGA_TEXT_H

#include <sys/types.h>

/// The address of the frame buffer.
#define VIDEO_ADDRESS   0xB8000
#define VGA_TEXT_WIDTH  80
#define VGA_TEXT_HEIGHT 25

#define COLOR_BLACK         0
#define COLOR_BLUE          1
#define COLOR_GREEN         2
#define COLOR_CYAN          3
#define COLOR_RED           4
#define COLOR_MAGENTA       5
#define COLOR_BROWN         6
#define COLOR_LIGHT_GREY    7
#define COLOR_DARK_GREY     8
#define COLOR_LIGHT_BLUE    9
#define COLOR_LIGHT_GREEN   10
#define COLOR_LIGHT_CYAN    11
#define COLOR_LIGHT_RED     12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_LIGHT_YELLOW  14
#define COLOR_WHITE         15

/**
 * Initializes the VGA text mode driver.
 */
void vga_text_init();

/**
 * Writes a character to the screen at (x, y).
 *
 * @param c the character to write to the screen
 * @param scheme the color scheme
 * @param x the X position on the screen
 * @param y the Y position on the screen
 */
void vga_text_write_at(char c, uint8_t scheme, uint8_t x, uint8_t y);

/**
 * Moves the cursor at (x, y).
 *
 * @param x the X position on the screen
 * @param y the Y position on the screen
 */
void vga_text_move_cursor(uint8_t x, uint8_t y);

/**
 * Disables the cursor.
 */
void vga_text_disable_cursor();

#endif
