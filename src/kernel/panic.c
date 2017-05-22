#include "panic.h"
#include <core/isr.h>
#include <stdio.h>

void kernel_panic(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    vprintf(DEVICE_SCREEN, format, arg);
    va_end(arg);

    printf("\nSystem halted!\n");

    // completely stop
    irq_disable();
    while (1) ;
}
