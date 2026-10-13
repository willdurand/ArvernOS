#include <boot.h>

#include <stddef.h>

static boot_info_t current = { 0 };

void boot_info_set(const boot_info_t* info)
{
  if (info == NULL) {
    return;
  }

  current = *info;
}

const boot_info_t* boot_info_get()
{
  return &current;
}
