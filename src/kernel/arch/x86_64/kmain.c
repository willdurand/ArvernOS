#include "kmain.h"
#include <assert.h>
#include <boot.h>
#include <console/console.h>
#include <core/multiboot.h>
#include <core/tss.h>
#include <drivers/serial.h>
#include <kmain.h>
#include <logging.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <string.h>

void load_modules(boot_info_t* info);

void load_modules(boot_info_t* info)
{
  // TODO: See also comment in `src/kernel/arch/x86_64/core/multiboot.c` about
  // the lack of support for more than one module.
  multiboot_tag_module_t* module = multiboot_find(MULTIBOOT_TAG_TYPE_MODULE);

  if (module != NULL && strcmp(module->cmdline, "initrd") == 0) {
    info->initrd_addr = (uintptr_t)module->mod_start;
    info->initrd_size = module->mod_end - module->mod_start;
  }
}

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

  boot_info_t info = {
    .initrd_addr = 0,
    .initrd_size = 0,
    .cmdline = multiboot_get_cmdline(),
  };

  load_modules(&info);

  if (console_mode_is_vbe()) {
    INFO("%s", "console: switching to fullscreen mode");

    if (!console_fullscreen()) {
      WARN("%s", "console: failed to switch to fullscreen mode");
    }
  }

  boot_info_set(&info);
  kmain_start();
}
