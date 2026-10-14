#include <board.h>
#include <boot.h>
#include <core/arm/atag.h>
#include <drivers/miniuart.h>
#include <kmain.h>
#include <kmain_start.h>
#include <logging.h>
#include <stdint.h>

void kmain(uintptr_t w0)
{
  miniuart_init(GPIO_BASE);
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

  boot_info_t info = { 0 };
  atag_get_boot_info(&info);

  kmain_start(&info);
}
