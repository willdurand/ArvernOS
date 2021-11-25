#include <inttypes.h>
#include <parg.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef enum action
{
  DO_NOTHING,
  PRINT_HELP,
  RUN_SHELL
} action_t;

int main(int argc, char* argv[])
{
  printf("init: %s started with pid=%" PRId64 "\n", argv[0], getpid());

  const char* options = "hs";
  struct parg_state ps;
  parg_init(&ps);

  action_t action = DO_NOTHING;

  int c;
  while ((c = parg_getopt(&ps, argc, argv, options)) != -1) {
    switch (c) {
      case 'h':
        action = PRINT_HELP;
        break;
      case 's': {
        action = RUN_SHELL;
        break;
      }
    }
  }

  switch (action) {
    case DO_NOTHING:
      printf("nothing to do...\n");
      break;
    case PRINT_HELP:
      printf("usage: init [-%s]\n\n", options);
      printf("  s : run the user shell\n");
      printf("  h : print this help message\n");
      printf("\n");
      break;
    case RUN_SHELL: {
      const char* path = "/bin/shell";
      // We cannot allocate memory right now so let's pass up to 9 parameters
      // to the program we are about to execute.
      char* args[10] = { NULL };
      args[0] = (char*)path;

      int i = 1;
      for (c = ps.optind; c < argc && i < 9; c++) {
        args[i++] = argv[c];
      }
      args[i] = NULL;

      printf("init: executing %s\n", path);

      return execv(path, args);
    }
  }

  while (1) {
    ;
  }

  return 0;
}
