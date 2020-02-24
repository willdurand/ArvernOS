/** @file */
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

/// All the I/O ports are calculated relative to the data port. This is because
/// all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
/// order, but they start at different values.

#define SERIAL_COM1 0x3F8

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/// Tells the serial port to expect first the highest 8 bits on the data port,
/// then the lowest 8 bits will follow.
#define SERIAL_LINE_ENABLE_DLAB 0x80

/// 115200 bauds.
#define SERIAL_SPEED_115200 1
/// 57600 bauds.
#define SERIAL_SPEED_57600 2
/// 38400 bauds.
#define SERIAL_SPEED_38400 3

/**
 * Initializes the serial driver.
 *
 * @param com a serial port number
 * @param speed the serial speed to use
 */
void serial_init(uint16_t com, uint16_t speed);

/**
 * Prints a string to the serial port.
 *
 * @param com a serial port number
 * @param str the string to print to the serial port
 */
void serial_print(uint16_t com, const char* str);

#ifdef ENABLE_KERNEL_DEBUG

/**
 * This function is only exposed when `ENABLE_KERNEL_DEBUG` is set to `true` so
 * that the `printf` lib can relies on the serial debug port to print debug
 * logs. The implementation of this function is designed for the `printf` lib.
 */
void serial_stream_output(char c, void* arg);

#endif

#endif
