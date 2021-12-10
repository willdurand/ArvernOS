#include "alloc.h"
#include <arvern/utils.h>
#include <inttypes.h>
#include <mmu/logging.h>
#include <mmu/paging.h>
#include <stdint.h>
#include <string.h>

#ifdef CONFIG_USE_DLMALLOC

#include <stdlib.h>

static uintptr_t heap_addr = HEAP_START;

void alloc_init()
{
  dlmalloc_set_footprint_limit(HEAP_SIZE);
}

void* libk_sbrk(ptrdiff_t size)
{
  DEBUG("heap_addr=%p size=%td", heap_addr, size);

  if (size == 0) {
    return (void*)heap_addr;
  } else if (size > 0) {
    map_multiple(page_containing_address(heap_addr),
                 size / PAGE_SIZE,
                 PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
    memset((void*)heap_addr, 0, size);
  }

  uintptr_t old = heap_addr;
  uintptr_t new = heap_addr + size;

  // This cannot happen in theory because `size` cannot be negative due to
  // `MORECORE_CANNOT_TRIM` being set to `1` in `dlmalloc.h`.
  if (new < HEAP_START) {
    return NULL;
  }

  heap_addr = new;

  return (void*)old;
}

uint64_t alloc_get_used_count()
{
  return dlmalloc_footprint() / PAGE_SIZE;
}

uint64_t alloc_get_max_count()
{
  return dlmalloc_footprint_limit() / PAGE_SIZE;
}

#else // CONFIG_USE_DLMALLOC

#include <mmu/bitmap.h>
#include <panic.h>
#include <sys/types.h>

static uint64_t heap_end_page = 0;
static uint64_t heap_start_page = 0;
static bitmap_t allocated_pages[(MAX_PAGES / BITS_PER_WORD) + 1] = { 0 };

void alloc_init()
{
  heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
  heap_start_page = page_containing_address(HEAP_START);

  INFO("initialized heap allocator with heap_start_page=%" PRIu64
       " heap_end_page=%" PRIu64 " "
       "used_count=%" PRIu64 "",
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

  int32_t free_page_count = 0;
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

  for (int32_t i = 0; i < number_of_pages; i++) {
    bitmap_set(allocated_pages, first_free_page.value + i);
  }

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

  unmap_multiple(page, number_of_pages);

  uint64_t first_free_page =
    page_containing_address((uint64_t)ptr - HEAP_START);
  for (int32_t i = 0; i < number_of_pages; i++) {
    bitmap_clear(allocated_pages, first_free_page + i);
  }

  MMU_DEBUG(
    "free'd ptr=%p page=%u number_of_pages=%d", ptr, page, number_of_pages);

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

#endif // CONFIG_USE_DLMALLOC
