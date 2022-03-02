#include "kshell.h"
#include <arch/io.h>
#include <keymap.h>
#include <logging.h>
#include <osinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>

void help(int argc, char* argv[]);
void kshell_print_prompt();
void reset_readline();
void run_command();

static char readline[READLINE_SIZE] = { 0 };
static char last_readline[READLINE_SIZE] = { 0 };
static unsigned int readline_index = 0;

#define NB_DOCUMENTED_COMMANDS 14

static const char* commands[][NB_DOCUMENTED_COMMANDS] = {
  { "cat", "print on the standard output" },
  { "date", "print the current date and time" },
  { "exec", "execute a program in user mode" },
  { "help", "print this help message" },
  { "host", "perform a DNS lookup" },
  { "ls", "list files" },
  { "net", "show configured network interfaces" },
  { "ntp", "get the time from a time server" },
  { "overflow", "test the stack buffer overflow protection" },
  { "ping", "ping an IPv4 address" },
  { "poweroff", "power off the system" },
  { "ps", "show information about the tasks" },
  { "selftest", "run the kernel test suite" },
  { "usermode", "switch to usermode (alias for 'exec /bin/init -s')" },
};

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
  } else if (strcmp(argv[0], "cat") == 0) {
    cat(argc, argv);
  } else if (strcmp(argv[0], "date") == 0) {
    date();
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
  } else if (strcmp(argv[0], "usermode") == 0) {
    char* args[] = { "exec", "/bin/init", "-s" };
    exec(3, args);
  } else if (strcmp(argv[0], "poweroff") == 0) {
    power_off(REBOOT_CMD_POWER_OFF);
  } else if (strcmp(argv[0], "ps") == 0) {
    ps(argc, argv);
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

void kshell(int argc, char* argv[])
{
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "selftest") == 0) {
        int retval = selftest();

        power_off(retval == 0 ? REBOOT_CMD_POWER_OFF_EXIT_CODE_0
                              : REBOOT_CMD_POWER_OFF_EXIT_CODE_1);
      } else {
        printf("kshell: unsupported argument: %s\n", argv[i]);
      }
    }
  }

  printf("\nThis is " KERNEL_NAME " kernel shell. "
         "Type 'help' for more information.\n"
         "Protip: switch to usermode with the 'usermode' command.\n\n");

  kshell_print_prompt();

  while (1) {
    unsigned char c = arch_getchar(true);

    switch (c) {
      case KEY_ARROW_UP:
        if (strlen(last_readline) > 0) {
          reset_readline();
          strncpy(readline, last_readline, READLINE_SIZE);
          memset(last_readline, 0, READLINE_SIZE);
          printf("%s", readline);
          readline_index = strlen(readline);
        }
        break;

      case 0x08:
      case 0x7f:
        if (readline_index > 0) {
          // destructive backspace
          printf("\b \b");
          readline_index--;
          readline[readline_index] = 0;
        }
        break;

      case '\n':
        printf("\n");
        strncpy(last_readline, readline, READLINE_SIZE);
        run_command();
        reset_readline();
        kshell_print_prompt();
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
