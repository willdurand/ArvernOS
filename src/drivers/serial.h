#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

// All the I/O ports are calculated relative to the data port. This is because
// all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
// order, but they start at different values.

#define SERIAL_COM1                     0x3F8

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

// Tells the serial port to expect first the highest 8 bits on the data port,
// then the lowest 8 bits will follow.
#define SERIAL_LINE_ENABLE_DLAB         0x80

#define SERIAL_SPEED_115200 1
#define SERIAL_SPEED_57600  2
#define SERIAL_SPEED_38400  3

void serial_init(uint16_t com, uint16_t divisor);
void serial_print(uint16_t com, const char* str);
void serial_write(uint16_t com, char c);
void serial_printf(uint16_t com, const char* format, ...);

#endif
