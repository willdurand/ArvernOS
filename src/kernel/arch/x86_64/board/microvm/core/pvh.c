#include "pvh.h"

#include <stddef.h>

static const char* cmdline = NULL;
static pvh_mmap_t mmap = { 0 };
static uintptr_t module_start = 0;
static uintptr_t module_end = 0;

void pvh_init(hvm_start_info_t* info)
{
  cmdline = (char*)info->cmdline_paddr;
  mmap.addr = info->memmap_paddr;
  mmap.count = info->memmap_entries;

  if (info->nr_modules > 0 && info->modlist_paddr != 0) {
    const struct hvm_modlist_entry* modlist =
      (const struct hvm_modlist_entry*)(uintptr_t)info->modlist_paddr;

    module_start = (uintptr_t)modlist[0].paddr;
    module_end = module_start;

    for (uint32_t i = 0; i < info->nr_modules; i++) {
      uintptr_t start = (uintptr_t)modlist[i].paddr;
      uintptr_t end = start + (uintptr_t)modlist[i].size;

      if (start < module_start) {
        module_start = start;
      }

      if (end > module_end) {
        module_end = end;
      }
    }
  }
}

const char* pvh_get_cmdline()
{
  return cmdline;
}

pvh_mmap_t* pvh_get_mmap()
{
  return &mmap;
}

void pvh_get_modules_range(uintptr_t* start, uintptr_t* end)
{
  *start = module_start;
  *end = module_end;
}

void pvh_get_boot_info(boot_info_t* info)
{
  info->initrd_addr = module_start;
  info->initrd_size = (uint64_t)(module_end - module_start);
  info->cmdline = pvh_get_cmdline();
}
