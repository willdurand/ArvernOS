#include <kmain.h>

#include <kshell/kshell.h>
#include <logging.h>
#include <osinfo.h>
#include <panic.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>

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

void kmain_start(int argc, char* argv[])
{
  if (strcmp(argv[0], "kshell") == 0) {
    printf("kernel: loading %s...\n", argv[0]);
    INFO("kernel: loading %s...", argv[0]);

    kshell(argc, argv);
  } else {
    printf("kernel: switching to usermode... (%s)\n", argv[0]);
    INFO("kernel: switching to usermode... (%s)", argv[0]);

    k_execv(argv[0], argv);
  }

  PANIC("unexpectedly reached end of kmain");
}
