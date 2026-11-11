#include <boot.h>

#include <stddef.h>
#include <string.h>

static boot_info_t current = { 0 };

void boot_info_set(const boot_info_t* info)
{
  if (info == NULL) {
    return;
  }

  current = *info;

  // The command line reported by the boot loader may not outlive early boot,
  // so keep our own copy. `boot_info_set()` runs once the heap is available.
  if (info->cmdline != NULL) {
    char* copy = strdup(info->cmdline);

    if (copy != NULL) {
      current.cmdline = copy;
    }
  }
}

const boot_info_t* boot_info_get()
{
  return &current;
}
