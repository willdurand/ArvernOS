#include <stdio.h>

char** environ = NULL;

#ifndef __is_libk

int start_main(int (*main)(int, char**, char**),
               int argc,
               char* argv[],
               char* envp[])
{
#ifdef DEBUG_LIBC
  printf(
    "start_main: main=%p argc=%d argv=%p envp=%p\n", main, argc, argv, envp);
#endif

  environ = envp;

  return main(argc, argv, envp);
  // TODO: call `exit()`
}

#endif
