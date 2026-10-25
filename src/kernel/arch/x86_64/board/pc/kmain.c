#include <kmain.h>

#include <assert.h>
#include <boot.h>
#include <console/console.h>
#include <core/multiboot.h>
#include <core/tss.h>
#include <drivers/serial.h>
#include <kmain_start.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>

void kmain(uintptr_t addr)
{
  multiboot_info_t* mbi = (multiboot_info_t*)addr;
  assert(mbi != NULL);
  multiboot_init(mbi);

  // enable serial port
  serial_init(SERIAL_COM1, SERIAL_SPEED_115200);

  multiboot_tag_framebuffer_t* entry = multiboot_get_framebuffer();
  assert(entry != NULL);
  console_init(&entry->common);

  kmain_early_start();

  tss_init();
  frame_init();
  paging_init();
  alloc_init();

  boot_info_t info = { 0 };
  multiboot_get_boot_info(&info);

  if (console_mode_is_vbe()) {
    INFO("%s", "console: switching to fullscreen mode");

    if (!console_fullscreen()) {
      WARN("%s", "console: failed to switch to fullscreen mode");
    }
  }

  kmain_start(&info);
}
