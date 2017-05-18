#include "screen.h"

uint8_t new_scheme(uint8_t fg, uint8_t bg)
{
    return fg | bg << 4;
}

uint16_t vga_print(uint32_t c, uint8_t scheme)
{
    uint16_t c16 = c;
    uint16_t scheme16 = scheme;

    return c16 | scheme16 << 8;
}
