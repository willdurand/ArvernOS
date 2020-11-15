#include "paging.h"
#include <kernel/panic.h>
#include <mmu/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MMU_DEBUG_PAGE_ENTRY(message, e)                                       \
  MMU_DEBUG("%s "                                                              \
            "page entry addr=%p present=%d "                                   \
            "writable=%d no_execute=%d user_accessible=%d",                    \
            message,                                                           \
            (e).addr,                                                          \
            (e).present,                                                       \
            (e).writable,                                                      \
            (e).no_execute,                                                    \
            (e).user_accessible)

void zero_table(page_table_t* table);
page_table_t* next_table_address(page_table_t* table, uint64_t index);
uint64_t translate(uint64_t virtual_address);
opt_uint64_t pointed_frame(page_entry_t entry);
uint64_t p4_index(uint64_t page);
page_table_t* next_table_create(page_table_t* table, uint64_t index);
page_table_t* get_p4();
void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags);
uint32_t paging_flags_for_entry(page_entry_t* entry);
opt_uint64_t paging_frame_allocate();

page_table_t* p4 = (page_table_t*)P4_TABLE;

void paging_init(multiboot_info_t* mbi)
{
  // UNUSED(*mbi);
}

void zero_table(page_table_t* table)
{
  memset((void*)table, 0, sizeof(page_table_t));

  for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
    table->entries[i] = (page_entry_t){ .present = 0 };
  }
}

page_table_t* next_table_address(page_table_t* table, uint64_t index)
{
  if (table->entries[index].present != 1) {
    MMU_DEBUG("entry at index=%u in table=%p is not present, returning 0",
              index,
              table);
    return 0;
  }

  if (table->entries[index].huge_page == 1) {
    MMU_DEBUG(
      "huge page detected for table=%p index=%u, returning 0", table, index);
    // TODO(william): fix this problem. Huge page is detected and we
    // are not able to create a page table because of this. This
    // prevents an ELF to load... That's pretty bad :(
    return 0;
  }

#ifdef TEST_ENV
  page_table_t* next_table = test_next_table();
#else
  page_table_t* next_table =
    (page_table_t*)(((uint64_t)table << 9) | (index << 12));
#endif

  MMU_DEBUG("addr=%p, next table=%p", (uint64_t)table, next_table);

  return next_table;
}

// Translates a virtual address to the corresponding physical
// address.
uint64_t translate(uint64_t virtual_address)
{
  uint64_t offset = virtual_address % PAGE_SIZE;
  uint64_t page = page_containing_address(virtual_address);
  opt_uint64_t frame = translate_page(page);

  return frame.value * PAGE_SIZE + offset;
}

uint64_t page_containing_address(uint64_t virtual_address)
{
  // So the address space is split into two halves: the higher half
  // containing addresses with sign extension and the lower half
  // containing addresses without. Everything in between is invalid.
  if (virtual_address < 0x0000800000000000 ||
      virtual_address >= 0xffff800000000000) {
    return virtual_address / PAGE_SIZE;
  }

  PANIC("invalid address=%p", virtual_address);
  return 0; // never reached but avoid a GCC warning.
}

uint64_t page_start_address(uint64_t page_number)
{
  return page_number * PAGE_SIZE;
}

opt_uint64_t translate_page(uint64_t page_number)
{
  page_table_t* p4 = get_p4();

  page_table_t* p3 = next_table_address(p4, p4_index(page_number));

  if (p4->entries[p4_index(page_number)].huge_page) {
    opt_uint64_t frame = pointed_frame(p3->entries[_p3_index(page_number)]);

    MMU_DEBUG("1GB huge page=%u frame=%u", page_number, frame.value);

    if (frame.value % (PAGE_ENTRIES * PAGE_ENTRIES) == 0) {
      frame.value +=
        _p2_index(page_number) * PAGE_ENTRIES + _p1_index(page_number);

      return frame;
    }

    PANIC("misaligned 1GB page=%u", page_number);
  }

  if (p3 == 0) {
    DEBUG("did not find p3 (%p), returning no value", p3);
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  page_table_t* p2 = next_table_address(p3, _p3_index(page_number));

  if (p3->entries[_p3_index(page_number)].huge_page) {
    opt_uint64_t frame = pointed_frame(p2->entries[_p2_index(page_number)]);

    MMU_DEBUG("2MB huge page=%u frame=%u", page_number, frame.value);

    if (frame.value % PAGE_ENTRIES == 0) {
      frame.value += _p1_index(page_number);

      return frame;
    }

    PANIC("misaligned 2MB page=%u", page_number);
  }

  if (p2 == 0) {
    DEBUG("did not find p2 (%p), returning no value", p2);
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  page_table_t* p1 = next_table_address(p2, _p2_index(page_number));

  if (p1 == 0) {
    DEBUG("did not find p1 (%p), returning no value", p1);
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  return pointed_frame(p1->entries[_p1_index(page_number)]);
}

uint64_t p4_index(uint64_t page)
{
  return (page >> 27) & 0777;
}

uint64_t _p3_index(uint64_t page)
{
  return (page >> 18) & 0777;
}

uint64_t _p2_index(uint64_t page)
{
  return (page >> 9) & 0777;
}

uint64_t _p1_index(uint64_t page)
{
  return (page >> 0) & 0777;
}

opt_uint64_t pointed_frame(page_entry_t entry)
{
  MMU_DEBUG_PAGE_ENTRY("pointed_frame", entry);

  if (entry.present) {
    return (opt_uint64_t){ .has_value = true,
                           .value = frame_containing_address(entry.addr) };
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

void map_page_to_frame(uint64_t page, uint64_t frame, uint64_t flags)
{
  DEBUG("mapping page=%u to frame=%p (number=%d) with flags=%#x",
        page,
        frame,
        frame_containing_address(frame),
        flags);

  uint64_t frame_number = frame_containing_address(frame);

  opt_uint64_t maybe_frame = translate_page(page);
  if (maybe_frame.has_value && maybe_frame.value == frame_number) {
    DEBUG("page=%u already mapped to frame=%u", page, frame_number);
    return;
  }

  uint64_t p4_idx = p4_index(page);
  uint64_t p3_idx = _p3_index(page);
  uint64_t p2_idx = _p2_index(page);
  uint64_t p1_idx = _p1_index(page);

  MMU_DEBUG(
    "p1_idx=%u p2_idx=%u p3_idx=%u p4_idx=%u", p1_idx, p2_idx, p3_idx, p4_idx);

  MMU_DEBUG("p4=%p", get_p4());

  MMU_DEBUG("%s", "get or create p3");
  page_table_t* p3 = next_table_create(get_p4(), p4_idx);
  MMU_DEBUG("p3=%p", p3);

  MMU_DEBUG("%s", "get or create p2");
  page_table_t* p2 = next_table_create(p3, p3_idx);
  MMU_DEBUG("p2=%p", p2);

  MMU_DEBUG("%s", "get or create p1");
  page_table_t* p1 = next_table_create(p2, p2_idx);
  MMU_DEBUG("p1=%p", p1);

  page_entry_t* entry = &p1->entries[p1_idx];

  if (entry->addr != 0 &&
      paging_flags_for_entry(entry) != (flags | PAGING_FLAG_PRESENT)) {
    PANIC("%s", "entry should be unused");
  }

  MMU_DEBUG("frame=%p (number=%d)...", frame, frame_containing_address(frame));
  MMU_DEBUG_PAGE_ENTRY("before set", (*entry));

  paging_set_entry(entry, (uint64_t)frame, flags | PAGING_FLAG_PRESENT);

  MMU_DEBUG_PAGE_ENTRY("after set", (*entry));

  p1->entries[p1_idx] = *entry;

  MMU_DEBUG("mapped page=%u to frame=%p (number=%d)...",
            page,
            frame,
            frame_containing_address(frame));
  MMU_DEBUG_PAGE_ENTRY("...with", p1->entries[p1_idx]);

  MMU_DEBUG("entry addr=%p in p1[%d]", p1->entries[p1_idx].addr, p1_idx);
}

page_table_t* next_table_create(page_table_t* table, uint64_t index)
{
  MMU_DEBUG("table=%p index=%d", table, index);

  bool was_created = false;

  page_entry_t entry = table->entries[index];

  if (entry.present) {
    MMU_DEBUG("page entry at index=%d is present", index);
  } else {
    MMU_DEBUG("page entry at index=%d is not present, creating entry", index);

    opt_uint64_t frame = paging_frame_allocate();

    if (!frame.has_value) {
      PANIC("%s", "frame is unexpectedly missing");
    }

    paging_set_entry(
      &entry, frame.value, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

    table->entries[index] = entry;

    MMU_DEBUG_PAGE_ENTRY("created", table->entries[index]);

    was_created = true;
  }

  page_table_t* next_table = next_table_address(table, index);
  MMU_DEBUG("next table=%p", next_table);

  if (was_created) {
    zero_table(next_table);
    MMU_DEBUG("zero'ed table=%p", next_table);
  }

  return next_table;
}

void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags)
{
  if ((addr & !0x000ffffffffff000) != 0) {
    PANIC("trying to set an invalid address: %p", addr);
  }

  entry->addr = addr;

  if (flags & PAGING_FLAG_PRESENT) {
    entry->present = 1;
  }

  if (flags & PAGING_FLAG_WRITABLE) {
    entry->writable = 1;
  }

  if (flags & PAGING_FLAG_NO_EXECUTE) {
    entry->no_execute = 1;
  }

  MMU_DEBUG_PAGE_ENTRY("set", (*entry));
}

page_table_t* get_p4()
{
  return p4;
}

void _set_p4(page_table_t* table)
{
  MMU_DEBUG("setting p4 to addr=%p", table);

  p4 = table;
}

void unmap(uint64_t page_number)
{
  uint64_t addr = page_start_address(page_number);

  if (translate(addr) == 0) {
    DEBUG("cannot unmap page=%u (start_address=%p) because it is "
          "not mapped",
          page_number,
          addr);
  }

  page_table_t* p4 = get_p4();
  page_table_t* p3 = next_table_address(p4, p4_index(page_number));
  page_table_t* p2 = next_table_address(p3, _p3_index(page_number));
  page_table_t* p1 = next_table_address(p2, _p2_index(page_number));

  page_entry_t entry = p1->entries[_p1_index(page_number)];
  opt_uint64_t frame = pointed_frame(entry);
  uint64_t frame_number = frame.value;

  memset(&p1->entries[_p1_index(page_number)], 0, sizeof(page_entry_t));
  MMU_DEBUG_PAGE_ENTRY("cleared", p1->entries[_p1_index(page_number)]);

  // TODO(william): free p(1,2,3) table if empty

  frame_deallocate(frame_number);

  // flush the translation lookaside buffer
  // cf. http://os.phil-opp.com/modifying-page-tables.html#unmap
  __asm__("invlpg (%0)" : /* no output */ : "r"(addr) : "memory");

  MMU_DEBUG("unmapped page=%u addr=%p", page_number, addr);
}

void map(uint64_t page_number, uint64_t flags)
{
  opt_uint64_t frame = paging_frame_allocate();

  if (!frame.has_value) {
    PANIC("out of memory");
  }

  MMU_DEBUG("mapping page=%d", page_number);

  map_page_to_frame(page_number, frame.value, flags);
}

void map_multiple(uint64_t start_page_number,
                  uint32_t number_of_pages,
                  uint64_t flags)
{
  MMU_DEBUG("mapping %d pages, starting at page=%d",
            number_of_pages,
            start_page_number);

  for (uint32_t i = 0; i < number_of_pages; i++) {
    map(start_page_number + i, flags);
  }
}

void unmap_multiple(uint64_t start_page_number, uint32_t number_of_pages)
{
  MMU_DEBUG("unmapping %d pages, starting at page=%d",
            number_of_pages,
            start_page_number);

  for (uint32_t i = 0; i < number_of_pages; i++) {
    unmap(start_page_number + i);
  }
}

uint32_t paging_amount_for_byte_size(uint64_t start_address, uint64_t byte_size)
{
  uint64_t start_page = page_containing_address(start_address);
  uint64_t end_page = page_containing_address(start_address + byte_size);

  uint32_t difference = (uint32_t)(end_page - start_page);

  if (difference == 0) {
    return 1;
  }

  return difference;
}

uint32_t paging_flags_for_entry(page_entry_t* entry)
{
  uint32_t flags = 0;

  if (entry->present) {
    flags |= PAGING_FLAG_PRESENT;
  }

  if (entry->no_execute) {
    flags |= PAGING_FLAG_NO_EXECUTE;
  }

  if (entry->writable) {
    flags |= PAGING_FLAG_WRITABLE;
  }

  return flags;
}

opt_uint64_t paging_frame_allocate()
{
#ifdef TEST_ENV
  return test_frame_allocate();
#endif

  return frame_allocate();
}
