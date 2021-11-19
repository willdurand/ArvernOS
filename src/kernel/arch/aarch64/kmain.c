#include "kmain.h"

#include <config/inish.h>
#include <drivers/miniuart.h>
#include <fs/debug.h>
#include <fs/dev.h>
#include <fs/sock.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <kmain.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/k_syscall.h>

extern unsigned char _binary___initrd_tar_start;

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

  print_step("mounting tarfs (init ramdisk)");
  inode_t initrd =
    vfs_mount("/", tar_fs_create((uintptr_t)&_binary___initrd_tar_start));

  if (initrd) {
    print_ok();

    print_step("  mounting devfs");
    if (dev_fs_init()) {
      print_ok();
    } else {
      print_ko();
    }

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
