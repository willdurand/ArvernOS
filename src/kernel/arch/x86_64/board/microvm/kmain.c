#include <kmain.h>

#include <assert.h>
#include <boot.h>
#include <core/pvh.h>
#include <core/tss.h>
#include <drivers/serial.h>
#include <kmain_start.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>

void kmain(uintptr_t addr)
{
  hvm_start_info_t* hsi = (hvm_start_info_t*)addr;
  assert(hsi != NULL);
  pvh_init(hsi);

  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
  DEBUG("microvm: addr=%p cmdline=%s", addr, pvh_get_cmdline());

  kmain_early_start();

  tss_init();
  frame_init();
  paging_init();
  alloc_init();

  boot_info_t info = { 0 };
  pvh_get_boot_info(&info);

  kmain_start(&info);
}
