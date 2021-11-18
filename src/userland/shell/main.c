#include "shell.h"
#include <arvern/utils.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READLINE_SIZE          256
#define PROMPT                 "\033[0;36m$\033[0m "
#define NB_DOCUMENTED_COMMANDS 4

void print_prompt();

static const char* commands[][NB_DOCUMENTED_COMMANDS] = {
  { "env", "print the environment variables" },
  { "help", "print this help message" },
  { "reboot", "reboot the system" },
  { "selftest", "run the user mode test suite" },
};

static char readline[READLINE_SIZE] = { 0 };
static unsigned int readline_index = 0;

void print_prompt()
{
  printf(PROMPT);
}

int main(int argc, char* argv[])
{
  UNUSED(argc);
  UNUSED(**argv);

  putenv("USER=user");
  putenv("SHELL=/bin/shell");

  printf("shell: pid=%d\n", getpid());
  printf("\nWelcome!\n"
         "This is ArvernOS user shell. Type 'help' for more information.\n\n");

  print_prompt();

  while (1) {
    unsigned char c = getchar();

    switch (c) {
      case 0:
        break;

      case '\b':
        if (readline_index == 0) {
          break;
        }

        // destructive backspace
        printf("\b \b");

        if (readline_index > 0) {
          readline_index--;
        }

        readline[readline_index] = 0;

        break;

      case '\n':
        printf("\n");

        if (*readline == 0) {
          print_prompt();
          break;
        }

        char* command = strtok(readline, " ");
        char* first_arg = strtok(NULL, " ");

        // Comsume all other arguments.
        while (strtok(NULL, " ") != NULL) {
          ;
        }

        if (strcmp(command, "help") == 0) {
          if (first_arg == NULL) {
            for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
              printf("%-10s %s\n", commands[i][0], commands[i][1]);
            }
          } else {
            for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
              if (strcmp(first_arg, commands[i][0]) == 0) {
                printf("%s - %s\n", first_arg, commands[i][1]);
                break;
              }
            }
          }
        } else if (strcmp(command, "reboot") == 0) {
          _reboot();
        } else if (strcmp(command, "selftest") == 0) {
          selftest();
        } else if (strcmp(command, "env") == 0) {
          env();
        } else {
          printf("invalid command\n");
        }

        readline_index = 0;
        memset(readline, 0, READLINE_SIZE);

        print_prompt();

        break;

      case '\t':
        printf("  ");
        readline[readline_index++] = 0;
        readline[readline_index++] = 0;

        break;

      default:
        if (c < 128) {
          printf("%c", c);
          readline[readline_index++] = c;
        }
    }
  }

  return 0;
}
