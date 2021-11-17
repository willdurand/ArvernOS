#include <arch/io.h>

#include <console/console.h>
#include <drivers/keyboard.h>
#include <keymap.h>

static bool caps_lock_mode = false;
static bool shift_mode = false;

unsigned char arch_getchar(bool blocking)
{
  unsigned char c = 0;

  while (c == 0) {
    uint8_t scancode = keyboard_get_scancode();
    bool key_was_released = false;

    if (scancode > 128) {
      key_was_released = true;
      scancode -= 128;
    }

    switch (scancode) {
      case 0:
        // no input
        if (!blocking) {
          return 0;
        }
        break;

      case KB_CAPS_LOCK:
        if (key_was_released) {
          caps_lock_mode = !caps_lock_mode;
        }
        break;

      case KB_LSHIFT:
      case KB_RSHIFT:
        if (key_was_released) {
          shift_mode = false;
        } else {
          shift_mode = true;
        }
        break;

      default:
        if (key_was_released) {
          switch (scancode) {
            case KB_ARROW_UP:
              return KEY_ARROW_UP;
          }

          if ((caps_lock_mode || shift_mode) && keymap[scancode][1]) {
            c = keymap[scancode][1];
          } else {
            c = keymap[scancode][0];
          }
        }
    }
  }

  return c;
}

void arch_putchar(char c)
{
  console_putchar(c);
}
