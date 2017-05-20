#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <drivers/screen.h>

char *convert(unsigned int num, int base);
void putchar(char c);
void puts(char *s);

void printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);

    int i_val;
    for (int i = 0; i < strlen(format); i++) {
        char c = format[i];

        if (c == '%') {
            switch (format[i + 1]) {
                case 'c':
                    putchar(va_arg(arg, int));
                    break;

                case 'd':
                    i_val = va_arg(arg, int);
                    if (i_val < 0) {
                        i_val = -i_val;
                        putchar('-');
                    }
                    puts(convert(i_val, 10));
                    break;

                case 'x':
                    i_val = va_arg(arg, unsigned int);
                    puts(convert(i, 16));
                    break;

                case 's':
                    puts(va_arg(arg,char *));
                    break;
            }

            i++;
        } else {
            putchar(c);
        }
    }

    va_end(arg);
}

char *convert(unsigned int num, int base)
{
    char representation[] = "0123456789ABCDEF";
    char buffer[50];
    char *ret;

    ret = &buffer[49];
    *ret = '\0';

    do {
        *--ret = representation[num % base];
        num /= base;
    } while(num != 0);

    return ret;
}

void putchar(char c)
{
    screen_write(c);
}

void puts(char *s)
{
    screen_print(s);
}
