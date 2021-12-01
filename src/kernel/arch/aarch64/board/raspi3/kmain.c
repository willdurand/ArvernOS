#include "kmain.h"
#include <core/atag.h>
#include <drivers/miniuart.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <kmain.h>
#include <logging.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/k_syscall.h>

extern unsigned char _binary___initrd_tar_start;

void kmain(uintptr_t w0)
{
  miniuart_init();
  kmain_print_banner();

  // 0 = on real hardware, w0 should be 0 when device trees are used (the
  // default) but QEMU sets w0 to 0x100 and passes ATAGs when we use `-append`
  // so we have to deal with that, too.
  if (w0 == 0 || w0 == 0x100) {
    atag_init((atag_header_t*)0x100);
  } else {
    DEBUG("w0=%p but we do not support DTB", w0);
  }

  print_step("initializing heap allocator");
  // nothing to do for now
  print_ok();

  print_step("initializing syscalls");
  syscall_init();
  print_ok();

  kmain_init_fs((uintptr_t)&_binary___initrd_tar_start);

  kmain_start(atag_get_cmdline());
}
