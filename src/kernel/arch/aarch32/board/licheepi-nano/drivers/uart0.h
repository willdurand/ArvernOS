#ifndef DRIVERS_UART0_H
#define DRIVERS_UART0_H

void uart0_init();

void uart0_putc(char c);

void uart0_puts(const char* str);

char uart0_getc();

#endif
