#include "kshell.h"
#include <logging.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char readline[READLINE_SIZE] = { 0 };
static char last_readline[READLINE_SIZE] = { 0 };
static unsigned int readline_index = 0;

static bool caps_lock_mode = false;
static bool ctrl_mode = false;
static bool shift_mode = false;

#define NB_DOCUMENTED_COMMANDS 9

static const char* commands[][NB_DOCUMENTED_COMMANDS] = {
  { "exec", "execute a program in user mode" },
  { "help", "print this help message" },
  { "host", "perform a DNS lookup" },
  { "ls", "list files" },
  { "net", "show configured network interfaces" },
  { "ntp", "get the time from a time server" },
  { "overflow", "test the stack buffer overflow protection" },
  { "ping", "ping an IPv4 address" },
  { "selftest", "run the kernel test suite" },
};

static unsigned char keymap[][128] = {
  { 0 },         { 27 },       { '1', '!' }, { '2', '@' },  { '3', '#' },
  { '4', '$' },  { '5', '%' }, { '6', '^' }, { '7', '&' },  { '8', '*' },
  { '9', '(' },  { '0', ')' }, { '-', '_' }, { '=', '+' },  { '\b' },
  { '\t' },      { 'q', 'Q' }, { 'w', 'W' }, { 'e', 'E' },  { 'r', 'R' },
  { 't', 'T' },  { 'y', 'Y' }, { 'u', 'U' }, { 'i', 'I' },  { 'o', 'O' },
  { 'p', 'P' },  { '[', '{' }, { ']', '}' }, { '\n' },      { 0 },
  { 'a', 'A' },  { 's', 'S' }, { 'd', 'D' }, { 'f', 'F' },  { 'g', 'G' },
  { 'h', 'H' },  { 'j', 'J' }, { 'k', 'K' }, { 'l', 'L' },  { ';', ':' },
  { '\'', '"' }, { '`', '~' }, { 0 },        { '\\', '|' }, { 'z', 'Z' },
  { 'x', 'X' },  { 'c', 'C' }, { 'v', 'V' }, { 'b', 'B' },  { 'n', 'N' },
  { 'm', 'M' },  { ',', '<' }, { '.', '>' }, { '/', '?' },  { 0 },
  { '*' },       { 0 },        { ' ' },      { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { '-' },
  { 0 },         { 0 },        { 0 },        { '+' },       { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },
};

unsigned char get_char(uint8_t scancode, bool shift, bool caps_lock)
{
  if ((caps_lock || shift) && keymap[scancode][1]) {
    return keymap[scancode][1];
  }

  return keymap[scancode][0];
}

void help(int argc, char* argv[])
{
  if (argc < 2) {
    for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
      printf("%-10s %s\n", commands[i][0], commands[i][1]);
    }

    return;
  }

  for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
    if (strncmp(argv[1], commands[i][0], strlen(commands[i][0])) == 0) {
      printf("%s - %s\n", argv[1], commands[i][1]);
      return;
    }
  }

  printf("no help for this command\n");
}

void run_command()
{
  if (*readline == 0) {
    return;
  }

  int argc = 1;
  char* _readline = strdup(readline);
  strtok(_readline, " ");
  while (strtok(NULL, " ") != NULL) {
    argc++;
  }
  free(_readline);

  char** argv = malloc(sizeof(char*) * argc);
  argv[0] = strtok(readline, " ");
  DEBUG("command='%s' argc=%d", argv[0], argc);

  for (int i = 1; i < argc; i++) {
    argv[i] = strtok(NULL, " ");
    DEBUG("argv[%d]=%s", i, argv[i]);
  }

  if (strcmp(argv[0], "help") == 0) {
    help(argc, argv);
  } else if (strcmp(argv[0], "ls") == 0) {
    ls(argc, argv);
  } else if (strcmp(argv[0], "selftest") == 0) {
    selftest();
  } else if (strcmp(argv[0], "overflow") == 0) {
    overflow();
  } else if (strcmp(argv[0], "net") == 0) {
    net(argc, argv);
  } else if (strcmp(argv[0], "ping") == 0) {
    ping(argc, argv);
  } else if (strcmp(argv[0], "host") == 0) {
    host(argc, argv);
  } else if (strcmp(argv[0], "ntp") == 0) {
    ntp(argc, argv);
  } else if (strcmp(argv[0], "exec") == 0) {
    exec(argc, argv);
  } else {
    printf("invalid kshell command\n");
  }

  free(argv);
}

void kshell_print_prompt()
{
  printf("\033[0;36m%s\033[0m", PROMPT);
}

void reset_readline()
{
  readline_index = 0;

  for (unsigned int i = 0; i < READLINE_SIZE; i++) {
    readline[i] = 0;
  }
}

void kshell_run(uint8_t scancode)
{
  if (scancode == 0) {
    return;
  }

  bool key_was_released = false;

  if (scancode > 128) {
    key_was_released = true;
    scancode -= 128;
  }

  switch (scancode) {
    case KB_CAPS_LOCK:
      if (key_was_released) {
        caps_lock_mode = !caps_lock_mode;
      }

      break;

    case KB_ARROW_UP:
      if (key_was_released && strlen(last_readline) > 0) {
        reset_readline();
        strncpy(readline, last_readline, READLINE_SIZE);
        memset(last_readline, 0, READLINE_SIZE);
        printf("%s", readline);
        readline_index = strlen(readline);
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
      if (key_was_released && readline_index > 0) {
        // destructive backspace
        printf("\b \b");
        readline_index--;
        readline[readline_index] = 0;
      }

      break;

    case KB_ENTER:
      if (key_was_released) {
        printf("\n");
        strncpy(last_readline, readline, READLINE_SIZE);
        run_command();
        reset_readline();
        kshell_print_prompt();
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
        unsigned char c = get_char(scancode, shift_mode, caps_lock_mode);

        if (c) {
          // Handle keyboard shortcuts.
          if (ctrl_mode) {
            switch (c) {
              case 'c':
                readline[readline_index++] = '^';
                readline[readline_index++] = 'C';
                printf("^C\n");
                kshell_print_prompt();
                break;

              case 'l':
                printf("\033[H\033[2J");
                reset_readline();
                kshell_print_prompt();
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
