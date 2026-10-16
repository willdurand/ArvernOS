/** @file */
#ifndef DRIVERS_SERIAL_H
#define DRIVERS_SERIAL_H

#include <stdbool.h>
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

/**
 * Prints a character to the serial port.
 *
 * @param com a serial port number
 * @param c the character to print to the serial port
 */
void serial_write(uint16_t com, char c);

/**
 * Reads a character from the serial port. This function blocks until a
 * character is available.
 *
 * @param com a serial port number
 * @return the character
 */
char serial_read(uint16_t com);

/**
 * Tells whether a character is waiting in the serial port receive queue.
 *
 * @param com a serial port number
 * @return whether a character is available
 */
bool serial_is_waiting(uint16_t com);

#endif
