#include "alloc.h"
#include <kernel/panic.h>
#include <mmu/debug.h>
#include <mmu/paging.h>
#include <string.h>
#include <sys/types.h>

uint64_t heap_end_page;
uint64_t heap_start_page;
uint64_t next_free_page;

void alloc_init()
{
  heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
  heap_start_page = page_containing_address(HEAP_START);
  next_free_page = 0;

  DEBUG("heap_start_page=%u heap_end_page=%u", heap_start_page, heap_end_page);
}

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
  opt_uint64_t first_free_page = { .has_value = false, .value = 0 };

  if (next_free_page + number_of_pages < MAX_PAGES) {
    first_free_page.has_value = true;
    first_free_page.value = next_free_page;
  }

  if (!first_free_page.has_value) {
    PANIC("no free pages for alloc of %d pages", number_of_pages);
  }

  uint64_t addr = page_start_address(heap_start_page + first_free_page.value);

  next_free_page += number_of_pages;

  map_multiple(heap_start_page + first_free_page.value,
               number_of_pages,
               PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

  MMU_DEBUG("allocated %d pages at addr=%p", number_of_pages, addr);

  return (void*)addr;
}

int liballoc_free(void* ptr, int number_of_pages)
{
  uint64_t page = page_containing_address((uint64_t)ptr);
  MMU_DEBUG("ptr=%p page=%u", ptr, page);

  // TODO: we cannot decrement `next_free_page` for now. We should use a bitmap
  // in order to reclaim free'd pages.

  unmap_multiple(page, number_of_pages);

  MMU_DEBUG(
    "free'd ptr=%p page=%u number_of_pages=%d", ptr, page, number_of_pages);

  return 0;
}
