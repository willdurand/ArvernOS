#include "shell.h"
#include <core/cmos.h>
#include <core/debug.h>
#include <drivers/screen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

unsigned char keymap[128] = {
    0,
    27,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n',
    0,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0, // Left shift
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0, // Right shift
    '*',
    0, // Alt
    ' ',
    0, // Caps lock
    0, // F1
    0, // F2
    0, // F3
    0, // F4
    0, // F5
    0, // F6
    0, // F8
    0, // F7
    0, // F9
    0, // F10
    0, // Num lock
    0, // Scroll lock
    0, // Home key
    0, // Arrow up
    0, // Page up
    '-',
    0, // Arrow left
    0,
    0, // Arrow right
    '+',
    0, // End
    0, // Arrow down
    0, // Page down
    0, // Insert ket
    0, // Deltee key
    0,
    0,
    0,
    0, // F11
    0, // F12
    0,
};

#define NB_DOCUMENTED_COMMANDS 5

const char* commands[][NB_DOCUMENTED_COMMANDS] = {
    {"help", "display information about willOS shell commands"},
    {"date", "print the system date and time"},
    {"clear", "clear the terminal screen"},
    {"reboot", "stopping and restarting the system"},
};

unsigned char get_char(uint8_t scancode) {
    return keymap[scancode];
}

void help(const char* command) {
    const char* arg = 0;

    if (strlen(command) == 4) {
        arg = command;
    } else {
        arg = command + 5;
    }

    for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
        if (strncmp(arg, commands[i][0], strlen(commands[i][0])) == 0) {
            printf("%s - %s\n", arg, commands[i][1]);
            return;
        }
    }

    printf("no help for this command\n");
}

void date() {
    cmos_rtc_t rtc = cmos_read_rtc();

    printf("%4d-%2d-%2d %2d:%2d:%2d UTC\n",
           rtc.year,
           rtc.month,
           rtc.day,
           rtc.hours,
           rtc.minutes,
           rtc.seconds
          );
}

void clear() {
    screen_clear();
}

extern void reboot();

void run_command(const char* command) {
    DEBUG("command: %s", command);

    if (*command == 0) {
        return;
    }

    // TODO: implement and use strktok() to get the command and the arguments.

    if (strncmp(command, "help", 4) == 0) {
        help(command);
    } else if (strncmp(command, "date", 4) == 0) {
        date();
    } else if (strncmp(command, "clear", 5) == 0) {
        clear();
    } else if (strncmp(command, "reboot", 6) == 0) {
        reboot();
    } else {
        printf("invalid command\n");
    }
}

unsigned char readline[READLINE_SIZE] = {0};
unsigned int readline_index = 0;

bool ctrl_mode = false;
bool shift_mode = false;

void reset_readline() {
    readline_index = 0;

    for (unsigned int i = 0; i < READLINE_SIZE; i++) {
        readline[i] = 0;
    }

    printf(PROMPT);
}

void shell(uint8_t scancode) {
    bool key_was_released = false;

    if (scancode > 128) {
        key_was_released = true;
        scancode -= 128;
    }

    switch (scancode) {
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
        if (key_was_released && readline_index > 0) {
            printf("\b");
            readline_index--;
            readline[readline_index] = 0;
        }

        break;

    case KB_ENTER:
        if (key_was_released) {
            printf("\n");
            run_command((const char*)readline);
            reset_readline();
        }

        break;

    case KB_TAB:
        if (key_was_released) {
            printf("  ");
            readline[readline_index++] = 0;
            readline[readline_index++] = 0;
        }

        break;

    default:
        if (key_was_released) {
            unsigned char c = get_char(scancode);

            if (c) {
                // Handle keyboard shortcuts.
                if (ctrl_mode) {
                    switch (c) {
                    case 'c':
                        readline[readline_index++] = '^';
                        readline[readline_index++] = 'C';
                        printf("^C\n");
                        printf(PROMPT);
                        break;

                    case 'l':
                        clear();
                        reset_readline();
                        break;
                    }
                } else {
                    printf("%c", c);
                    readline[readline_index++] = c;
                }
            }
        }
    }
}
