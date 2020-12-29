#include "shell.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>

#define READLINE_SIZE          256
#define PROMPT                 "\033[0;36m(sh)\033[0m "
#define NB_DOCUMENTED_COMMANDS 3

void print_selftest_header(const char* name);

void print_selftest_header(const char* name)
{
  printf("\n\033[1;33m[%s]\033[0m\n", name);
}

void selftest()
{
  print_selftest_header("syscall");
  printf("  syscalling\n");
  test("usermode");

  print_selftest_header("filesystem");
  int fd = open("/dev/debug", O_RDWR);
  const char* message = "  this message should be written to the console\n";
  write(fd, message, strlen(message));
  close(fd);

  printf("\ndone.\n");
}

void main()
{
  const char* commands[][NB_DOCUMENTED_COMMANDS] = {
    { "help", "display information about shell commands" },
    { "reboot", "stopping and restarting the system" },
    { "selftest", "run the system test suite" },
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

        // destructive backspace
        printf("\b \b");

        if (readline_index >= 1) {
          readline_index--;
        }

        readline[readline_index] = 0;

        break;

      case '\n':
        printf("\n");

        if (*readline == 0) {
          printf(PROMPT);
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
        } else {
          printf("invalid command\n");
        }

        readline_index = 0;
        memset(readline, 0, READLINE_SIZE);

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
}
