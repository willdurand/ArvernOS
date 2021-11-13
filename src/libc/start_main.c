#include <stdio.h>
#include <willos/log.h>

// TODO: use malloc() when it becomes possible
char* environ[10] = { NULL };

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

  // TODO: add the entries in `envp` to `environ`.

  int retval = main(argc, argv, envp);

  WILLOS_LOG("retval=%d", retval);

#ifdef ENABLE_USERLAND_DEBUG
  willos_log_deinit();
#endif

  // TODO: call `exit()`
  return retval;
}

#endif
