#include <arvern/log.h>
#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>

// TODO: Use malloc() when it becomes possible.
char* environ[10] = { NULL };

#ifndef __is_libk

void start_main(int (*main)(int, char**, char**), int argc, char* argv[])
{
  errno = 0;

  char** envp = argv + argc + 1;

#ifdef ENABLE_USERLAND_DEBUG
  arvern_log_init();
  ARVERN_LOG("main=%p argc=%d argv=%p envp=%p", main, argc, argv, envp);
#endif

  // TODO: Add the entries in `envp` to `environ`.

  int retval = main(argc, argv, envp);

#ifdef ENABLE_USERLAND_DEBUG
  ARVERN_LOG("retval=%d", retval);
  arvern_log_deinit();
#endif

  exit(retval);
}

#endif
