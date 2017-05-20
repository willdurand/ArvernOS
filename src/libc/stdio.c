#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
    char s_val[20];
    for (int i = 0; i < strlen(format); i++) {
        char c = format[i];

        if (c == '%') {
            switch (format[i + 1]) {
                case 'c':
                    putchar(device, va_arg(arg, int));
                    break;

                case 'u':
                    u_val = va_arg(arg, uint32_t);
                    itoa(u_val, s_val, 10);
                    puts(device, s_val);
                    break;

                case 'd':
                    i_val = va_arg(arg, int32_t);
                    itoa(i_val, s_val, 10);
                    puts(device, s_val);
                    break;

                case 'x':
                    i_val = va_arg(arg, uint32_t);
                    itoa(i_val, s_val, 16);
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
