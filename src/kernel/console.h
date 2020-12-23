/** @file */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <core/multiboot.h>
#include <stdbool.h>
#include <stddef.h>
#include <vtconsole/vtconsole.h>

#define CONSOLE_WIDTH  80
#define CONSOLE_HEIGHT 25

#define CONSOLE_FONT_WIDTH  8
#define CONSOLE_FONT_HEIGHT 16

/**
 * Initializes the console (VT100).
 */
void console_init(multiboot_tag_framebuffer_common_t* common);

/**
 * Prints a character to the console.
 *
 * @param c the character to print
 */
void console_putchar(char c);

/**
 * Writes a string to the console.
 *
 * @param s the string to write to the console
 * @param count the number of characters to write
 * @return the number of characters printed to the console
 */
size_t console_write(const char* s, size_t count);

/**
 * Indicates whether the console is initialized with VBE mode.
 *
 * @return `true` if the mode is VBE (graphics), `false` otherwise
 */
bool console_mode_is_vbe();

/**
 * Switches a console into "fullscreen", i.e. respects the VBE dimensions
 * instead of the default one.
 *
 * @return `true` if the mode is VBE (graphics), `false` otherwise
 */
bool console_fullscreen();

// Functions for the "VGA text mode"-based console.
void vga_console_init();
void vga_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y);
void vga_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur);

// Functions for the VBE-based console.
void vbe_console_init(multiboot_tag_framebuffer_common_t* common);
void vbe_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y);
void vbe_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur);

#endif
