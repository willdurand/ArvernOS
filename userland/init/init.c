#include "init.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define READLINE_SIZE          256
#define PROMPT                 "(init) "
#define NB_DOCUMENTED_COMMANDS 9

int main()
{
  const char* commands[][NB_DOCUMENTED_COMMANDS] = {
    { "cal", "displays a calendar" },
    { "clear", "clear the terminal screen" },
    { "date", "print the system date and time" },
    { "exit", "exit the shell" },
    { "help", "display information about shell commands" },
    { "hostname", "print or update the system hostname" },
    { "reboot", "stopping and restarting the system" },
    { "uptime", "tell how long the system has been running" },
    { "overflow", "test the stack buffer overflow protection" },
  };

  char readline[READLINE_SIZE] = { 0 };
  unsigned int readline_index = 0;

  printf(PROMPT);

  while (1) {
    char c = getchar();

    switch (c) {
      case '\b':
        if (readline_index == 0) {
          break;
        }

        printf("\b");

        if (readline_index >= 1) {
          readline_index--;
        }

        readline[readline_index] = 0;

        break;

      case '\n':
        if (*readline == 0) {
          break;
        }

        printf("\n");

        char* command = strtok(readline, " ");
        char* first_arg = strtok(NULL, " ");

        // Comsume all other arguments.
        while (strtok(NULL, " ") != NULL) {
          ;
        }

        if (strcmp(command, "exit") == 0) {
          return 0;
        } else if (strcmp(command, "help") == 0) {
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
        } else if (strcmp(command, "cal") == 0) {
          cal();
        } else if (strcmp(command, "date") == 0) {
          date();
        } else if (strcmp(command, "clear") == 0) {
          clear();
        } else if (strcmp(command, "hostname") == 0) {
          hostname(first_arg);
        } else if (strcmp(command, "reboot") == 0) {
          _reboot();
        } else if (strcmp(command, "uptime") == 0) {
          uptime();
        } else if (strcmp(command, "overflow") == 0) {
          overflow();
        } else {
          printf("invalid command\n");
        }

        readline_index = 0;

        for (unsigned int i = 0; i < READLINE_SIZE; i++) {
          readline[i] = 0;
        }

        printf(PROMPT);

        break;

      case '\t':
        printf("  ");
        readline[readline_index++] = 0;
        readline[readline_index++] = 0;

        break;

      default:
        printf("%c", c);
        readline[readline_index++] = c;
    }
  }

  return 0;
}
