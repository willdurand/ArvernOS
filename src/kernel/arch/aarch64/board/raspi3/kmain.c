#include "kmain.h"
#include <core/arm/atag.h>
#include <drivers/miniuart.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <kmain.h>
#include <logging.h>
#include <stdint.h>
#include <stdio.h>

extern unsigned char _binary___initrd_tar_start;

void kmain(uintptr_t w0)
{
  miniuart_init();
  kmain_early_start();

  // 0 = on real hardware, w0 should be 0 when device trees are used (the
  // default) but QEMU sets w0 to 0x100 and passes ATAGs when we use `-append`
  // so we have to deal with that, too.
  uint32_t dtb_or_atags = (uint32_t)w0 & 0xFFFFFFFF;
  if (dtb_or_atags == 0 || dtb_or_atags == 0x100) {
    atag_init((atag_header_t*)0x100);
  } else {
    DEBUG("dtb_or_atags=%p but we do not support DTB", dtb_or_atags);
  }

  print_step("initializing heap allocator");
  // nothing to do for now
  print_ok();

  kmain_start((uintptr_t)&_binary___initrd_tar_start, atag_get_cmdline());
}
