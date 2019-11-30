#ifndef SHELL_H
#define SHELL_H

// This is required to use `uint8_t`.
#include <stdlib.h>

#define READLINE_SIZE   256
#define PROMPT          "$ "
#define KB_BACKSPACE    14
#define KB_ENTER        28
#define KB_LCTRL        29
#define KB_LSHIFT       42
#define KB_RSHIFT       54
#define KB_TAB          15

void shell(uint8_t scancode);

#endif
