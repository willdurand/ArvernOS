#include <arch/mmu.h>

#include <mmu/page.h>
#include <panic.h>

page_number_t arch_page_containing_address(uint64_t virtual_address)
{
  // So the address space is split into two halves: the higher half containing
  // addresses with sign extension and the lower half containing addresses
  // without. Everything in between is invalid.
  if (virtual_address < 0x0000800000000000 ||
      virtual_address >= 0xffff800000000000) {
    return virtual_address / PAGE_SIZE;
  }

  PANIC("invalid address=%p", virtual_address);
  return 0; // never reached but avoid a compiler error.
}
