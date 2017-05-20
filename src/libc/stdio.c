#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <drivers/screen.h>
#include <drivers/serial.h>

char *convert(unsigned int num, int base);
void putchar(int device, char c);
void puts(int device, char *s);

void printf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    vprintf(DEVICE_SCREEN, format, arg);
    va_end(arg);
}

void vprintf(int device, const char* format, va_list arg)
{

    int i_val;
    char s_val[20];
    for (int i = 0; i < strlen(format); i++) {
        char c = format[i];

        if (c == '%') {
            switch (format[i + 1]) {
                case 'c':
                    putchar(device, va_arg(arg, int));
                    break;

                case 'd':
                    i_val = va_arg(arg, int);
                    if (i_val < 0) {
                        i_val = -i_val;
                        putchar(device, '-');
                    }
                    itoa(i_val, s_val);
                    puts(device, s_val);
                    break;

                case 'x':
                    i_val = va_arg(arg, unsigned int);
                    puts(device, convert(i_val, 16));
                    break;

                case 's':
                    puts(device, va_arg(arg,char *));
                    break;
            }

            i++;
        } else {
            putchar(device, c);
        }
    }
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

void putchar(int device, char c)
{
    if (device > DEVICE_SCREEN) {
        serial_write(device, c);
    } else {
        screen_write(c);
    }
}

void puts(int device, char *s)
{
    if (device > DEVICE_SCREEN) {
        serial_print(device, s);
    } else {
        screen_print(s);
    }
}
