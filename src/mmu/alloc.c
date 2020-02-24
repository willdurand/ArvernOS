#include "alloc.h"
#include <kernel/panic.h>
#include <mmu/bitmap.h>
#include <mmu/debug.h>
#include <mmu/paging.h>
#include <string.h>

bitmap_t allocated_pages[HEAP_SIZE / PAGE_SIZE];
uint64_t heap_end_page;
uint64_t heap_start_page;
uint64_t max_pages;

void alloc_init()
{
  heap_end_page = page_containing_address(HEAP_START + HEAP_SIZE - 1);
  heap_start_page = page_containing_address(HEAP_START);
  max_pages = heap_end_page - heap_start_page + 1;

  for (uint64_t i = 0; i < HEAP_SIZE / PAGE_SIZE; i++) {
    allocated_pages[i] = 0;
  }

  DEBUG("heap_start_page=%u heap_end_page=%u max_pages=%u",
        heap_start_page,
        heap_end_page,
        max_pages);
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
  uint64_t first_free_page = 0;
  uint32_t free_page_count = 0;

  for (uint64_t i = 1; i <= max_pages; i++) {
    if (bitmap_get(allocated_pages, i) == false) {
      free_page_count++;

      if (free_page_count == number_of_pages) {
        first_free_page = i - (free_page_count - 1);
        break;
      }
    } else {
      free_page_count = 0;
    }
  }

  if (first_free_page == 0) {
    PANIC("no free pages for alloc of %d pages", number_of_pages);
  }

  uint64_t addr = page_start_address(heap_start_page + first_free_page);

  for (uint64_t i = 0; i < number_of_pages; i++) {
    bitmap_set(allocated_pages, first_free_page + i);
  }

  map_multiple(heap_start_page + first_free_page,
               number_of_pages,
               PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

  MMU_DEBUG("allocated %d pages at addr=%p", number_of_pages, addr);

  return (void*)addr;
}

int liballoc_free(void* ptr, int number_of_pages)
{
  uint64_t page = page_containing_address((uint64_t)ptr);
  MMU_DEBUG("ptr=%p page=%u", ptr, page);

  for (uint64_t i = 0; i < number_of_pages; i++) {
    bitmap_clear(allocated_pages, i);
  }

  unmap_multiple(page, number_of_pages);

  MMU_DEBUG(
    "free'ed ptr=%p page=%u number_of_pages=%d", ptr, page, number_of_pages);

  return 0;
}
