#include <sys/k_syscall.h>

#include <arvern/utils.h>
#include <stdlib.h>
#include <sys/logging.h>

#define PAGE_SIZE  4096
#define HEAP_PAGES 40

static void* heap = NULL;
static uint8_t heap_inc = 1;

void* k_mmap(void* addr,
             size_t length,
             int prot,
             int flags,
             int fd,
             off_t offset)
{
  MAYBE_UNUSED(length);
  MAYBE_UNUSED(prot);
  MAYBE_UNUSED(flags);
  MAYBE_UNUSED(fd);
  MAYBE_UNUSED(offset);

  SYS_DEBUG("addr=%p length=%d prot=%d flags=%d fd=%d offset=%d",
            addr,
            length,
            prot,
            flags,
            fd,
            offset);

  if (addr == NULL) {
    if (heap == NULL) {
      SYS_DEBUG("%s", "initializing heap");
      heap = malloc(PAGE_SIZE * HEAP_PAGES);

      return heap;
    }

    return &heap[PAGE_SIZE * heap_inc++];
  }

  return addr;
}
