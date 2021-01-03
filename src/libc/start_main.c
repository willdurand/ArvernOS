#include <stdio.h>
#include <willos/debug.h>

char** environ = NULL;

#ifndef __is_libk

int start_main(int (*main)(int, char**, char**),
               int argc,
               char* argv[],
               char* envp[])
{
  willos_debug(
    "start_main: main=%p argc=%d argv=%p envp=%p\n", main, argc, argv, envp);

  environ = envp;

  return main(argc, argv, envp);
  // TODO: call `exit()`
}

#endif
