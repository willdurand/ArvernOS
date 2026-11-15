#include <mmu/paging.h>

#include <arch/mmu.h>

uint64_t paging_page_to_address(page_number_t page_number)
{
  return page_number * PAGE_SIZE;
}

uint32_t paging_page_amount_for_byte_size(uint64_t start_address,
                                          uint64_t byte_size)
{
  page_number_t start_page = arch_paging_page_containing_address(start_address);
  page_number_t end_page =
    arch_paging_page_containing_address(start_address + byte_size);

  uint32_t difference = (uint32_t)(end_page - start_page) + 1;

  return difference;
}
