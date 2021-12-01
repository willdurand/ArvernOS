#ifndef DRIVERS_MINIUART_H
#define DRIVERS_MINIUART_H

void miniuart_init();

char miniuart_getc();

void miniuart_putc(char c);

void miniuart_puts(const char* str);

#endif
