#include <stdint.h>
#include <string.h>

extern unsigned char __heap_start;

static void* cur_brk = NULL;

void alloc_init()
{
  cur_brk = (void*)&__heap_start;
}

void* libk_sbrk(ptrdiff_t size)
{
  if (size == 0) {
    return cur_brk;
  }

  void* old = cur_brk;
  void* new = (void*)(cur_brk + size);

  if (new < (void*)&__heap_start) {
    return NULL;
  }

  cur_brk = new;

  if (size > 0) {
    memset(old, 0, size);
  }

  return old;
}
