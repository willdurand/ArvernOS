#include <stdio.h>
#include <willos/log.h>

char** environ = NULL;

#ifndef __is_libk

int start_main(int (*main)(int, char**, char**),
               int argc,
               char* argv[],
               char* envp[])
{
#ifdef ENABLE_USERLAND_DEBUG
  willos_log_init();
  WILLOS_LOG("main=%p argc=%d argv=%p envp=%p", main, argc, argv, envp);
#endif

  environ = envp;

  int retval = main(argc, argv, envp);

#ifdef ENABLE_USERLAND_DEBUG
  willos_log_deinit();
#endif

  // TODO: call `exit()`
  return retval;
}

#endif
