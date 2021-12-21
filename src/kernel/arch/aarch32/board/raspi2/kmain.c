#include "../../kmain.h"
#include <arvern/utils.h>
#include <board.h>
#include <core/arm/atag.h>
#include <drivers/miniuart.h>
#include <kmain.h>
#include <mmu/alloc.h>
#include <stdlib.h>

void kmain(uint32_t r0, uint32_t r1, uint32_t r2)
{
  UNUSED(r0);
  UNUSED(r1);

  miniuart_init(GPIO_BASE);
  kmain_early_start();

  atag_init((atag_header_t*)r2);
  alloc_init();

  // We load the init ramdisk as the first (and only) initrd2 entry.
  kmain_start(atag_get_initrd2_at(0)->start, atag_get_cmdline());
}
