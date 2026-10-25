#include <kmain.h>

#include <assert.h>
#include <core/pvh.h>
#include <core/tss.h>
#include <drivers/serial.h>
#include <kmain_start.h>
#include <logging.h>
#include <mmu/frame.h>

void kmain(uintptr_t addr)
{
  hvm_start_info_t* info = (hvm_start_info_t*)addr;
  assert(info != NULL);
  pvh_init(info);

  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
  DEBUG("microvm: addr=%p cmdline=%s", addr, pvh_get_cmdline());

  kmain_early_start();

  tss_init();
  frame_init();

  while (1) {
    __asm__ volatile("hlt");
  }

  // kmain_early_start();
  //
  // ...
  //
  // boot_info_t info = { 0 };
  // kmain_start(&info);
}
