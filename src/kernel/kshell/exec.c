#include <kshell/kshell.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

void exec(int argc, char* argv[])
{
  if (argc < 2) {
    printf("usage: %s <program> ...\n", argv[0]);
    return;
  }

  char** args = (char**)malloc(argc * sizeof(char*));
  for (int i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }
  args[argc - 1] = NULL;

  int retval = k_execv(args[0], args);

  printf("error trying to exec %s: %d\n", args[0], retval);
}
