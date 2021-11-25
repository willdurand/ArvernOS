#include <arvern/log.h>
#include <stdio.h>

// TODO: use malloc() when it becomes possible
char* environ[10] = { NULL };

#ifndef __is_libk

int start_main(int (*main)(int, char**, char**),
               int argc,
               char* argv[],
               char* envp[])
{
  arvern_log_init();
  ARVERN_LOG("main=%p argc=%d argv=%p envp=%p", main, argc, argv, envp);

  // TODO: add the entries in `envp` to `environ`.

  int retval = main(argc, argv, envp);

  ARVERN_LOG("retval=%d", retval);

  arvern_log_deinit();

  // TODO: call `exit()`
  return retval;
}

#endif
