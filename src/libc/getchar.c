#include <stdio.h>

#ifdef __is_libc

#include <proc/fd.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/syscall.h>

/// The scancode for the <kbd>arrow up</kbd> key.
#define KB_ARROW_UP 72
/// The scancode for the <kbd>backspace</kbd> key.
#define KB_BACKSPACE 14
/// The scancode for the <kbd>caps lock</kbd> key.
#define KB_CAPS_LOCK 58
/// The scancode for the <kbd>enter</kbd> key.
#define KB_ENTER 28
/// The scancode for the left <kbd>ctrl</kbd> key.
#define KB_LCTRL 29
/// The scancode for the left <kbd>shift</kbd> key.
#define KB_LSHIFT 42
/// The scancode for the right <kbd>shift</kbd> key.
#define KB_RSHIFT 54
/// The scancode for the <kbd>tab</kbd> key.
#define KB_TAB 15

char getchar()
{
  static unsigned char _keymap[][128] = {
    { 0 },         { 27 },       { '1', '!' }, { '2', '@' }, { '3', '#' },
    { '4', '$' },  { '5', '%' }, { '6', '^' }, { '7', '&' }, { '8', '*' },
    { '9', '(' },  { '0', ')' }, { '-', '_' }, { '=', '+' }, { '\b' },
    { '\t' },      { 'q', 'Q' }, { 'w', 'W' }, { 'e', 'E' }, { 'r', 'R' },
    { 't', 'T' },  { 'y', 'Y' }, { 'u', 'U' }, { 'i', 'I' }, { 'o', 'O' },
    { 'p', 'P' },  { '[', '{' }, { ']', '}' }, { '\n' },     { 0 },
    { 'a', 'A' },  { 's', 'S' }, { 'd', 'D' }, { 'f', 'F' }, { 'g', 'G' },
    { 'h', 'H' },  { 'j', 'J' }, { 'k', 'K' }, { 'l', 'L' }, { ';', ':' },
    { '\'', '"' }, { '`', '~' }, { 0 }, // Left shift
    { '\\', '|' }, { 'z', 'Z' }, { 'x', 'X' }, { 'c', 'C' }, { 'v', 'V' },
    { 'b', 'B' },  { 'n', 'N' }, { 'm', 'M' }, { ',', '<' }, { '.', '>' },
    { '/', '?' },  { 0 },                             // Right shift
    { '*' },       { 0 },                             // Alt
    { ' ' },                                          // Space
    { 0 },                                            // Caps lock
    { 0 },                                            // F1
    { 0 },                                            // F2
    { 0 },                                            // F3
    { 0 },                                            // F4
    { 0 },                                            // F5
    { 0 },                                            // F6
    { 0 },                                            // F8
    { 0 },                                            // F7
    { 0 },                                            // F9
    { 0 },                                            // F10
    { 0 },                                            // Num lock
    { 0 },                                            // Scroll lock
    { 0 },                                            // Home key
    { 0 },                                            // Arrow up
    { 0 },                                            // Page up
    { '-' },       { 0 },                             // Arrow left
    { 0 },         { 0 },                             // Arrow right
    { '+' },       { 0 },                             // End
    { 0 },                                            // Arrow down
    { 0 },                                            // Page down
    { 0 },                                            // Insert ket
    { 0 },                                            // Deltee key
    { 0 },         { 0 },        { 0 },        { 0 }, // F11
    { 0 },                                            // F12
    { 0 },
  };

  static bool caps_lock_mode = false;
  static bool ctrl_mode = false;
  static bool shift_mode = false;

  char c = -1;

  while (c == -1) {
    uint8_t scancode;
    read(FD_STDIN, &scancode, 1);
    bool key_was_released = false;

    if (scancode > 128) {
      key_was_released = true;
      scancode -= 128;
    }

    switch (scancode) {
      case 0:
        // no input
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

      case KB_LCTRL:
        if (key_was_released) {
          ctrl_mode = false;
        } else {
          ctrl_mode = true;
        }

        break;

      case KB_BACKSPACE:
        if (key_was_released) {
          c = '\b';
        }

        break;

      case KB_ENTER:
        if (key_was_released) {
          c = '\n';
        }

        break;

      case KB_TAB:
        if (key_was_released) {
          c = '\t';
        }

        break;

      default:
        if (key_was_released) {
          if ((caps_lock_mode || shift_mode) && _keymap[scancode][1]) {
            c = _keymap[scancode][1];
          } else {
            c = _keymap[scancode][0];
          }
        }
    }
  }

  return c;
}

#endif
