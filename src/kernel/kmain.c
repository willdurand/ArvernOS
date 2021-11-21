#include <kmain.h>

#include <kshell/kshell.h>
#include <logging.h>
#include <osinfo.h>
#include <panic.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>

char* saved_cmdline = NULL;

void print_step(const char* msg)
{
  printf("kernel: %-66s", msg);
  INFO("%s", msg);
}

void print_ok()
{
  printf("  [");
  printf("\033[0;32mOK\033[0m");
  printf("]\n");
}

void print_ko()
{
  printf("[");
  printf("\033[0;31mFAIL\033[0m");
  printf("]\n");
}

void kmain_print_banner()
{
  INFO("%s %s - %s (%s) / Built on: %s at %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       ARCH,
       GIT_HASH,
       KERNEL_DATE,
       KERNEL_TIME);

  printf("\033[1;34m%s\033[0m\n", KERNEL_ASCII);
  printf("%s %s - %s (%s) / Built on: %s at %s\n\n",
         KERNEL_NAME,
         KERNEL_VERSION,
         ARCH,
         GIT_HASH,
         KERNEL_DATE,
         KERNEL_TIME);
}

void kmain_start(const char* cmdline)
{
  if (cmdline == NULL) {
    PANIC("invalid cmdline passed to kmain_start()");
  }
  DEBUG("cmdline=%s", cmdline);
  // Keep a copy of the original command line.
  saved_cmdline = strdup(cmdline);

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
    printf("kernel: loading %s...\n", argv[0]);
    INFO("kernel: loading %s...", argv[0]);

    kshell(argc, argv);
  } else {
    printf("kernel: switching to usermode... (%s)\n", argv[0]);
    INFO("kernel: switching to usermode... (%s)", argv[0]);

    k_execv(argv[0], argv);
  }

  PANIC("unexpectedly reached end of kmain_start()");
}
