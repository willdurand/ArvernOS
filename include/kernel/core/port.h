/**
 * @file
 * @see https://wiki.osdev.org/I/O_Ports
 *
 * An I/O port is usually used as a technical term for a specific address on
 * the x86's IO bus, which is used to communicate with devices without
 * requiring memory access (alternative). This is the aim of this module.
 */
#ifndef CORE_PORT_H
#define CORE_PORT_H

#include <stdint.h>

/**
 * Reads a byte.
 *
 * @param port a port number
 * @return the value for the port number
 */
uint8_t port_byte_in(uint16_t port);

/**
 * Writes a byte to a port.
 *
 * @param port a port number
 * @param value the new value for the port
 */
void port_byte_out(uint16_t port, uint8_t value);

/**
 * Reads a word (2 bytes).
 *
 * @param port a port number
 * @return the value for the port number
 */
uint16_t port_word_in(uint16_t port);

/**
 * Writes a word to a port.
 *
 * @param port a port number
 * @param value the new value for the port
 */
void port_word_out(uint16_t port, uint16_t value);

/**
 * Reads a double word (4 bytes).
 *
 * @param port a port number
 * @return the value for the port number
 */
uint32_t port_dword_in(uint16_t port);

/**
 * Writes a double word to a port.
 *
 * @param port a port number
 * @param value the new value for the port
 */
void port_dword_out(uint16_t port, uint32_t value);

#endif
