#include <mmu/paging.h>

#include <arch/mmu.h>
#include <inttypes.h>
#include <logging.h>
#include <mmu/logging.h>
#include <panic.h>

#if defined(__x86_64__)

void paging_init()
{
  INFO("%s", "mmu: initialize paging");

  arch_paging_init();
}

void paging_map(page_number_t page_number, uint64_t flags)
{
  opt_uint64_t frame = arch_paging_allocate_frame();

  if (!frame.has_value) {
    PANIC("out of memory");
  }

  arch_paging_map_page(page_number, frame.value, flags);
}

void paging_map_multiple(page_number_t start_page_number,
                         uint32_t number_of_pages,
                         uint64_t flags)
{
  MMU_DEBUG("mapping %" PRIu32 " pages, starting at page=%" PRIu64,
            number_of_pages,
            start_page_number);

  for (uint32_t i = 0; i < number_of_pages; i++) {
    paging_map(start_page_number + i, flags);
  }
}

void paging_identity_map(uint64_t address, uint64_t flags)
{
  page_number_t page = paging_page_containing_address(address);

  arch_paging_map_page(page, paging_page_to_address(page), flags);
}

void* paging_create_user_space(void)
{
  return arch_paging_create_user_space();
}

void* paging_clone_user(void)
{
  return arch_paging_clone_user();
}

void paging_activate(void* space)
{
  arch_paging_activate(space);
}

void paging_deactivate(void* space)
{
  arch_paging_deactivate(space);
}

page_number_t paging_page_containing_address(uint64_t virtual_address)
{
  return arch_paging_page_containing_address(virtual_address);
}

#endif

uint64_t paging_page_to_address(page_number_t page_number)
{
  return page_number * PAGE_SIZE;
}

uint32_t paging_page_amount_for_byte_size(uint64_t start_address,
                                          uint64_t byte_size)
{
  page_number_t start_page = paging_page_containing_address(start_address);
  page_number_t end_page =
    paging_page_containing_address(start_address + byte_size);

  uint32_t difference = (uint32_t)(end_page - start_page) + 1;

  return difference;
}
