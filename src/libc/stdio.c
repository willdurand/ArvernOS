#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <drivers/screen.h>
#include <drivers/serial.h>

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
    int32_t i_val;
    uint32_t u_val;
    uint64_t l_val;
    char s_val[20];

    for (int i = 0; i < strlen(format); i++) {
        char c = format[i];

        if (c == '%') {
            uint8_t leftpad = format[i + 1] - '0';

            if (isdigit(leftpad)) {
                i++;
            } else {
                leftpad = 0;
            }

            switch (format[i + 1]) {
                case '%':
                    putchar(device, '%');
                    break;

                case 'c':
                    putchar(device, va_arg(arg, int));
                    break;

                case 'd':
                    i_val = va_arg(arg, int32_t);
                    itoa(i_val, s_val, 10);

                    while ((leftpad - strlen(s_val)) > 0) {
                        putchar(device, '0');
                        leftpad--;
                    }

                    puts(device, s_val);
                    break;

                case 'u':
                case 'x':
                    u_val = va_arg(arg, uint32_t);
                    itoa(u_val, s_val, format[i + 1] == 'x' ? 16 : 10);
                    puts(device, s_val);
                    break;

                case 'L':
                case 'X':
                    l_val = va_arg(arg, uint64_t);
                    ulltoa(l_val, s_val, format[i + 1] == 'X' ? 16 : 10);
                    puts(device, s_val);
                    break;

                case 's':
                    puts(device, va_arg(arg, char *));
                    break;
            }

            i++;
        } else {
            putchar(device, c);
        }
    }
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
