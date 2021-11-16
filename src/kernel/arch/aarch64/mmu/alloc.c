#include <arvern/utils.h>
#include <stdint.h>

#define PAGE_SIZE 4096

static uint64_t memtop = 0x400000;

int liballoc_lock()
{
  // not implemented
  return 0;
}

int liballoc_unlock()
{
  // not implemented
  return 0;
}

void* liballoc_alloc(int number_of_pages)
{
  memtop += (number_of_pages * PAGE_SIZE);

  return (void*)memtop;
}

int liballoc_free(void* ptr, int number_of_pages)
{
  UNUSED(*ptr);
  UNUSED(number_of_pages);

  return 0;
}
