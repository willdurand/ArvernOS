#include <mmu/logging.h>
#include <stdint.h>
#include <string.h>

static uint64_t memtop = 0x400000;

void* libk_sbrk(ptrdiff_t size)
{
  MMU_DEBUG("size=%td", size);

  if (size == 0) {
    return (void*)memtop;
  }

  uint64_t old = memtop;
  uint64_t new = memtop + size;

  if (new < 0x400000) {
    return NULL;
  }

  memtop = new;

  if (size > 0) {
    memset((void*)old, 0, size);
  }

  return (void*)old;
}
