#include "keyboard.h"
#include <core/debug.h>
#include <core/isr.h>
#include <core/port.h>
#include <stdlib.h>

#define RING_BUFFER_SIZE 8

uint8_t scancodes[RING_BUFFER_SIZE];
uint8_t buffer_length = 0;
uint8_t read_index = 0;
uint8_t write_index = 0;

static void keyboard_callback(stack_t* stack)
{
  uint8_t status = port_byte_in(KEYBOARD_STATUS_PORT);

  if (status & 0x01) {
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    DEBUG("received scancode: %d", scancode);

    if (buffer_length == RING_BUFFER_SIZE) {
      DEBUG("%s", "ring buffer is full");
      return;
    }

    scancodes[write_index++] = scancode;
    buffer_length++;

    if (write_index == RING_BUFFER_SIZE) {
      write_index = 0;
    }
  }

  UNUSED(*stack);
}

void keyboard_init()
{
  isr_register_handler(IRQ1, keyboard_callback);
}

uint8_t keyboard_get_scancode()
{
  if (buffer_length == 0) {
    return 0;
  }

  uint8_t scancode = scancodes[read_index++];
  buffer_length--;

  if (read_index == RING_BUFFER_SIZE) {
    read_index = 0;
  }

  return scancode;
}
