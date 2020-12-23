/** @file */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <core/multiboot.h>
#include <stddef.h>
#include <vtconsole/vtconsole.h>

#define CONSOLE_WIDTH  80
#define CONSOLE_HEIGHT 25

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

// Functions for the "VGA text mode"-based console.
void vga_console_init();
void vga_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y);
void vga_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur);

// Functions for the VBE-based console.
void vbe_console_init(multiboot_tag_framebuffer_common_t* common);
void vbe_on_paint_callback(vtconsole_t* vtc, vtcell_t* cell, int x, int y);
void vbe_on_move_callback(vtconsole_t* vtc, vtcursor_t* cur);

#endif
