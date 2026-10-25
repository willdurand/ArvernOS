#include <arvern/utils.h>
#include <kmain.h>

void kmain(uintptr_t addr)
{
  UNUSED(addr);

  const char* msg = "here!\n";
  while (*msg) {
    __asm__ volatile("outb %0, %1" : : "a"(*msg), "Nd"((uint16_t)0x3F8));
    msg++;
  }

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
