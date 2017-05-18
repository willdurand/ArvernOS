#include <stdint.h>
#include "kmain.h"

uint8_t new_color(uint8_t fg, uint8_t bg)
{
	return fg | bg << 4;
}

uint16_t vga_print(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;

	return c16 | color16 << 8;
}

void kmain()
{
    uint16_t* vga = (uint16_t*) 0xB8000;
    uint8_t color = new_color(COLOR_RED, COLOR_BLACK);

    vga[0] = vga_print('H', color);
    vga[1] = vga_print('A', color);
    vga[2] = vga_print('L', color);
    vga[3] = vga_print('L', color);
    vga[4] = vga_print('O', color);
}
