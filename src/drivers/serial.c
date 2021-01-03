#include "serial.h"
#include <core/port.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void serial_write(uint16_t com, char c);
bool serial_is_waiting(uint16_t com);
bool serial_is_transmitting(uint16_t com);
char serial_read(uint16_t com);

void serial_init(uint16_t com, uint16_t speed)
{
  port_byte_out(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
  port_byte_out(SERIAL_DATA_PORT(com), (speed >> 8) & 0x00FF);
  port_byte_out(SERIAL_DATA_PORT(com), speed & 0x00FF);

  // cf. https://littleosbook.github.io/#configuring-the-serial-port
  //
  // Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
  // Content: | d | b | prty  | s | dl  |
  // Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
  port_byte_out(SERIAL_LINE_COMMAND_PORT(com), 0x03);

  // Enable FIFO, clear them, with 14 bytes as size of queue
  port_byte_out(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);

  // Ready To Transmit (RTS) = 1, Data Terminal Ready (DTR) = 1, no interrupts
  port_byte_out(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

void serial_print(uint16_t com, const char* str)
{
  for (size_t i = 0; i < strlen(str); i++) {
    serial_write(com, str[i]);
  }
}

void serial_stream_output(char c, void* arg)
{
  uint16_t com = *(uint16_t*)arg;
  serial_write(com, c);
}

bool serial_is_transmitting(uint16_t com)
{
  /* 0x20 = 0010 0000 */
  return (port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 0x20);
}

bool serial_is_waiting(uint16_t com)
{
  return (port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 1);
}

char serial_read(uint16_t com)
{
  while (serial_is_waiting(com) == 0) {
    ;
  }

  return port_byte_in(com);
}

void serial_write(uint16_t com, char c)
{
  while (serial_is_transmitting(com) == 0) {
    ;
  }

  port_byte_out(com, c);
}
