#ifndef KSHELL_H
#define KSHELL_H

#include <stdlib.h>

#define READLINE_SIZE   256
#define PROMPT          "$ "
#define KB_ARROW_UP     72
#define KB_BACKSPACE    14
#define KB_CAPS_LOCK    58
#define KB_ENTER        28
#define KB_LCTRL        29
#define KB_LSHIFT       42
#define KB_RSHIFT       54
#define KB_TAB          15

void kshell(uint8_t scancode);
void print_prompt();

#endif
