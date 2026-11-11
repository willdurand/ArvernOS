#include <kmain_start.h>

#include <boot.h>
#include <initcall.h>
#include <kinit.h>
#include <logging.h>
#include <osinfo.h>
#include <panic.h>
#include <proc/task.h>
#include <stdio.h>

void kmain_early_start()
{
  INFO("\n\033[1;34m%s\033[0m\n", KERNEL_ASCII);
  INFO("%s %s (%s) for %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       KERNEL_GIT_HASH,
       KERNEL_TARGET);
}

void kmain_start(const boot_info_t* info)
{
  boot_info_set(info);

  const char* cmdline = boot_info_get()->cmdline;

  if (cmdline == NULL) {
    PANIC("no kernel command line was provided by the boot code");
  }
  INFO("kmain: cmdline is '%s'", cmdline);

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
