#include "alloc.h"
#include "logging.h"
#include <kernel/panic.h>
#include <mmu/bitmap.h>
#include <mmu/paging.h>
#include <string.h>
#include <sys/types.h>

uint64_t heap_end_page;
uint64_t heap_start_page;
bitmap_t allocated_pages[(MAX_PAGES / BITS_PER_WORD) + 1] = { 0 };

void alloc_init()
{
  heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
  heap_start_page = page_containing_address(HEAP_START);

  INFO("initialized heap allocator with heap_start_page=%u heap_end_page=%u "
       "used_count=%d",
       heap_start_page,
       heap_end_page,
       alloc_get_used_count());
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

  uint32_t free_page_count = 0;
  for (uint64_t i = 0; i < MAX_PAGES; i++) {
    if (bitmap_get(allocated_pages, i) == false) {
      free_page_count++;

      if (free_page_count == number_of_pages) {
        first_free_page.has_value = true;
        first_free_page.value = i - (number_of_pages - 1);
        break;
      }
    } else {
      free_page_count = 0;
    }
  }

  if (!first_free_page.has_value) {
    PANIC("no free pages for alloc of %d pages", number_of_pages);
  }

  uint64_t addr = page_start_address(heap_start_page + first_free_page.value);

  for (uint64_t i = 0; i < number_of_pages; i++) {
    bitmap_set(allocated_pages, first_free_page.value + i);
  }

  map_multiple(heap_start_page + first_free_page.value,
               number_of_pages,
               PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

  DEBUG("allocated %d pages at addr=%p", number_of_pages, addr);

  return (void*)addr;
}

int liballoc_free(void* ptr, int number_of_pages)
{
  uint64_t page = page_containing_address((uint64_t)ptr);
  MMU_DEBUG("ptr=%p page=%u", ptr, page);

  unmap_multiple(page, number_of_pages);

  uint64_t first_free_page =
    page_containing_address((uint64_t)ptr - HEAP_START);
  for (uint64_t i = 0; i < number_of_pages; i++) {
    bitmap_clear(allocated_pages, first_free_page + i);
  }

  DEBUG("free'd ptr=%p page=%u number_of_pages=%d", ptr, page, number_of_pages);

  return 0;
}

uint64_t alloc_get_used_count()
{
  uint64_t count = 0;

  for (uint64_t i = 0; i < MAX_PAGES; i++) {
    if (bitmap_get(allocated_pages, i)) {
      count++;
    }
  }

  return count;
}

uint64_t alloc_get_max_count()
{
  return MAX_PAGES;
}
