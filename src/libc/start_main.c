#include <arvern/log.h>
#include <stdio.h>

// TODO: Use malloc() when it becomes possible.
char* environ[10] = { NULL };

#ifndef __is_libk

int start_main(int (*main)(int, char**, char**),
               int argc,
               char* argv[],
               char* envp[])
{
#ifdef ENABLE_USERLAND_DEBUG
  arvern_log_init();
  ARVERN_LOG("main=%p argc=%d argv=%p envp=%p", main, argc, argv, envp);
#endif

  // TODO: Add the entries in `envp` to `environ`.

  int retval = main(argc, argv, envp);

  ARVERN_LOG("retval=%d", retval);

#ifdef ENABLE_USERLAND_DEBUG
  arvern_log_deinit();
#endif

  // TODO: Call `exit()` syscall.
  return retval;
}

#endif
