#include <kinit.h>

#include <boot.h>
#include <kshell/kshell.h>
#include <logging.h>
#include <stdlib.h>
#include <string.h>
#include <sys/k_syscall.h>

void kinit()
{
  const char* cmdline = boot_info_get()->cmdline;

  int argc = 0;
  char* _cmdline = strdup(cmdline);
  char* curr = strtok(_cmdline, " ");
  while (curr != NULL) {
    // We exclude command line arguments that contain a dot like
    // `module.param=value`.
    if (strchr(curr, '.') == 0) {
      argc++;
    }
    curr = strtok(NULL, " ");
  }
  free(_cmdline);

  char** argv = (char**)malloc(sizeof(char*) * (argc + 1));

  int _argc = 0;
  _cmdline = strdup(cmdline);
  curr = strtok(_cmdline, " ");
  while (curr != NULL) {
    if (strchr(curr, '.') == 0) {
      argv[_argc++] = strdup(curr);
    }
    curr = strtok(NULL, " ");
  }
  argv[_argc] = NULL;
  free(_cmdline);

  if (strcmp(argv[0], "kshell") == 0) {
    INFO("init: loading %s...", argv[0]);
    kshell(argc, argv);
  } else {
    INFO("init: switching to user mode... (%s)", argv[0]);
    // `k_execv()` takes over the current task on success and never returns.
    int retval = k_execv(argv[0], argv);

    WARN("init: cannot execute '%s' (ret=%d)", argv[0], retval);
  }

  k_exit(EXIT_FAILURE);
}
