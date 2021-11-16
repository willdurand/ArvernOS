#include "kmain.h"

#include <config/inish.h>
#include <drivers/miniuart.h>
#include <fs/debug.h>
#include <fs/dev.h>
#include <fs/sock.h>
#include <fs/vfs.h>
#include <kmain.h>
#include <stdio.h>
#include <sys/k_syscall.h>

void kmain()
{
  miniuart_init();
  kmain_print_banner();

  print_step("initializing heap allocator");
  // nothing to do for now
  print_ok();

  print_step("initializing syscalls");
  syscall_init();
  print_ok();

  print_step("initializing virtual file system");
  vfs_init();
  print_ok();

  // TODO: mount initrd at /

  print_step("  mounting devfs");
  if (dev_fs_init()) {
    print_ok();

    print_step("  mounting debugfs");
    if (debug_fs_init()) {
      print_ok();
    } else {
      print_ko();
    }
  }

  print_step("loading kernel.inish configuration");
  inish_config_t* kernel_cfg = inish_load("/etc/kernel.inish");

  if (kernel_cfg == NULL) {
    print_ko();
  } else {
    print_ok();
  }

  if (kernel_cfg != NULL) {
    inish_free(kernel_cfg);
  }

  kmain_start(1, (char*[]){ "kshell", NULL });
}
