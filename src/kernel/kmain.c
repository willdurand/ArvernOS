#include <kmain_start.h>

#include <boot.h>
#include <initcall.h>
#include <kshell/kshell.h>
#include <logging.h>
#include <osinfo.h>
#include <panic.h>
#include <proc/task.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>

char* saved_cmdline = NULL;

void kmain_early_start()
{
  INFO("\n\033[1;34m%s\033[0m\n", KERNEL_ASCII);
  INFO("%s %s (%s) for %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       KERNEL_GIT_HASH,
       KERNEL_TARGET);
}

void kinit()
{
  int argc = 0;
  char* _cmdline = strdup(saved_cmdline);
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
  _cmdline = strdup(saved_cmdline);
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
    INFO("kmain: loading %s...", argv[0]);
    kshell(argc, argv);
  } else {
    INFO("switching to user mode... (%s)", argv[0]);
    // `k_execv()` takes over the current task on success and never returns.
    int retval = k_execv(argv[0], argv);

    WARN("cannot execute '%s' (ret=%d)", argv[0], retval);
  }

  k_exit(EXIT_FAILURE);
}

void kmain_start(const boot_info_t* info)
{
  boot_info_set(info);

  const char* cmdline = boot_info_get()->cmdline;

  if (cmdline == NULL) {
    PANIC("no kernel command line was provided by the boot code");
  }
  INFO("kmain: cmdline is '%s'", cmdline);
  // Keep a copy of the original command line.
  saved_cmdline = strdup(cmdline);

  if (saved_cmdline == NULL) {
    PANIC("failed to copy cmdline, is heap memory available?");
  }

  initcall_init();

  task_init("idle");

  int res = task_create("kinit", KERNEL_TASK, (uintptr_t)&kinit);
  if (res < 0) {
    printf("kernel: error while starting kinit task\n");
  }

  kernel_boot_complete = true;

  while (1) {
    task_schedule();
  }

  PANIC("unexpectedly reached end of kmain_start()");
}
