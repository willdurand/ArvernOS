#ifndef DRIVERS_MINIUART_H
#define DRIVERS_MINIUART_H

#include <stdint.h>

void miniuart_init(uint32_t gpio_base_addr);

char miniuart_getc();

void miniuart_putc(char c);

void miniuart_puts(const char* str);

#endif
