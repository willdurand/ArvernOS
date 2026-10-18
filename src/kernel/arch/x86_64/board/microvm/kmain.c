#include <kmain.h>

#include <core/tss.h>
#include <drivers/serial.h>
#include <kmain_start.h>
#include <logging.h>

void kmain(uintptr_t addr)
{
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);
  DEBUG("microvm: addr=%p", addr);

  kmain_early_start();

  tss_init();

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
