#include "paging.h"
#include <kernel/panic.h>
#include <mmu/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MMU_DEBUG_PAGE_ENTRY(message, e)                                       \
  MMU_DEBUG("%s "                                                              \
            "page entry addr=%p present=%d "                                   \
            "writable=%d no_execute=%d",                                       \
            message,                                                           \
            (e).addr,                                                          \
            (e).present,                                                       \
            (e).writable,                                                      \
            (e).no_execute)

void zero_table(page_table_t* table);
page_table_t *next_table_address(page_table_t* table, uint64_t index);
opt_uint64_t translate(uint64_t virtual_address);
opt_uint64_t pointed_frame(page_entry_t entry);
uint64_t p4_index(uint64_t page);
uint64_t p3_index(uint64_t page);
uint64_t p2_index(uint64_t page);
uint64_t p1_index(uint64_t page);
page_table_t* next_table_create(page_table_t* table, uint64_t index);
page_table_t* get_p4();
void paging_set_entry(page_entry_t* entry, uint64_t addr, uint64_t flags);
uint32_t paging_flags_for_entry(page_entry_t* entry);

void paging_init(multiboot_info_t* mbi)
{
  UNUSED(*mbi);
}

void zero_table(page_table_t* table)
{
  memset((void*)table, 0, sizeof(page_table_t));
}

page_table_t *next_table_address(page_table_t* table, uint64_t index)
{
  if (table->entries[index].present != 1) {
    MMU_DEBUG("entry at index=%u in table=%p is not present, returning 0",
              index,
              table);

    return NULL;
  }

  if (table->entries[index].huge_page == 1) {
    MMU_DEBUG(
      "huge page detected for table=%p index=%u, returning 0", table, index);

    return NULL;
  }

  page_table_t* next_table =
    (page_table_t*)(((uint64_t)table << 9) | (index << 12));

  MMU_DEBUG("addr=%p next table=%p", (uint64_t)table, next_table);

  return next_table;
}

// Translates a virtual address to the corresponding physical address.
opt_uint64_t translate(uint64_t virtual_address)
{
  opt_uint64_t out_value;
  out_value.has_value = false;

  uint64_t offset = virtual_address % PAGE_SIZE;
  opt_uint64_t page = page_containing_address(virtual_address);

  if(page.has_value) {

    opt_uint64_t frame = translate_page(page.value);

    if(frame.has_value) {

      out_value.value = frame.value * PAGE_SIZE + offset;
      out_value.has_value = true;

      return out_value;
    }
  }

  return out_value;
}

opt_uint64_t page_containing_address(uint64_t virtual_address)
{
  opt_uint64_t out_value;
  out_value.has_value = false;

  // So the address space is split into two halves: the higher half
  // containing addresses with sign extension and the lower half containing
  // addresses without. Everything in between is invalid.
  if (virtual_address < 0x0000800000000000 ||
      virtual_address >= 0xffff800000000000) {

    out_value.value = virtual_address / PAGE_SIZE;
    out_value.has_value = true;

    MMU_DEBUG("page_containing_address returning %lld for %p", out_value.value, out_value.value);

    return out_value;
  }

  PANIC("invalid address=%p", virtual_address);
  return out_value; // never reached but avoid a GCC warning.
}

uint64_t page_start_address(uint64_t page_number)
{
  return page_number * PAGE_SIZE;
}

opt_uint64_t translate_page(uint64_t page_number)
{
  opt_uint64_t out_value;
  out_value.has_value = false;
  page_table_t* p4 = get_p4();

  page_table_t* p3 = next_table_address(p4, p4_index(page_number));

  if (p3 == NULL) {
    MMU_DEBUG("did not find p3 (%p), returning NULL", p3);
    return out_value;
  }

  page_entry_t *p3_entry = &p3->entries[p3_index(page_number)];

  opt_uint64_t start_frame = pointed_frame(*p3_entry);

  if(start_frame.has_value && p3_entry->huge_page == 1) {
    MMU_DEBUG("1GB huge page=%u frame=%u", page_number, start_frame.value);

    if(start_frame.value % (PAGE_ENTRIES * PAGE_ENTRIES) == 0) {
      out_value.value = start_frame.value + p2_index(page_number) * PAGE_ENTRIES + p1_index(page_number);
      out_value.has_value = true;

      return out_value;
    } else {
      PANIC("misaligned 1GB page=%u", page_number);
    }
  }

  page_table_t* p2 = next_table_address(p3, p3_index(page_number));

  if(p2 != NULL) {

    page_entry_t *p2_entry = &p2->entries[p2_index(page_number)];

    start_frame = pointed_frame(*p2_entry);

    if(start_frame.has_value && p2_entry->huge_page == 1) {
      MMU_DEBUG("2MB huge page=%u frame=%u", page_number, start_frame.value);

      if(start_frame.value % PAGE_ENTRIES == 0) {
        out_value.value = start_frame.value + p1_index(page_number);
        out_value.has_value = true;

        return out_value;
      } else {
        PANIC("misaligned 2MB page=%u", page_number);
      }
    }
  } else {
    MMU_DEBUG("did not find p2 (%p), returning NULL", p2);
    return out_value;
  }

  page_table_t* p1 = next_table_address(p2, p2_index(page_number));

  if (p1 == NULL) {
    MMU_DEBUG("did not find p1 (%p), returning NULL", p1);
    return out_value;
  }

  return pointed_frame(p1->entries[p1_index(page_number)]);
}

uint64_t p4_index(uint64_t page)
{
  return (page >> 27) & 0777;
}

uint64_t p3_index(uint64_t page)
{
  return (page >> 18) & 0777;
}

uint64_t p2_index(uint64_t page)
{
  return (page >> 9) & 0777;
}

uint64_t p1_index(uint64_t page)
{
  return (page >> 0) & 0777;
}

opt_uint64_t pointed_frame(page_entry_t entry)
{
  opt_uint64_t out_value;
  out_value.has_value = false;

  if (entry.present) {
    out_value.value = frame_containing_address(entry.addr);
    out_value.has_value = true;

    return out_value;
  }

  return out_value;
}

void map_page_to_frame(uint64_t page, uint64_t frame, uint64_t flags)
{
  MMU_DEBUG("mapping page=%u to frame=%p (number=%d) with flags=%#x",
            page,
            frame,
            frame_containing_address(frame),
            flags);

  uint64_t frame_number = frame_containing_address(frame);

  opt_uint64_t translated_page = translate_page(page);

  if(translated_page.has_value && translated_page.value == frame_number) {
    MMU_DEBUG("page=%u already mapped to frame=%u", page, frame_number);

    return;
  }

  uint64_t p4_idx = p4_index(page);
  uint64_t p3_idx = p3_index(page);
  uint64_t p2_idx = p2_index(page);
  uint64_t p1_idx = p1_index(page);

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

  page_entry_t entry = p1->entries[p1_idx];

  if (entry.addr != 0) {
    PANIC("%s", "entry should be unused");
  }

  paging_set_entry(&entry, frame, flags | PAGING_FLAG_PRESENT);

  p1->entries[p1_idx] = entry;

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

  page_table_t* next_table = next_table_address(table, index);
  MMU_DEBUG("next table=%p", next_table);

  if(next_table == NULL) {

    if(table->entries[index].huge_page) {
      PANIC("mapping code does not support huge pages");
    }

    uint64_t frame = frame_allocate();

    if (frame == 0) {
      PANIC("%s", "frame is unexpectedly equal to 0");
    }

    page_entry_t *entry = &table->entries[index];

    paging_set_entry(entry, frame, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

    MMU_DEBUG_PAGE_ENTRY("created", table->entries[index]);

    was_created = true;

    next_table = next_table_address(table, index);

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
  return (page_table_t*)P4_TABLE;
}

void unmap(uint64_t page_number)
{
  uint64_t addr = page_start_address(page_number);

  if (!translate(addr).has_value) {
    PANIC("cannot unmap page=%u because it is not mapped", page_number);
  }

  page_table_t* p4 = get_p4();

  if(p4 == NULL) {
    PANIC("unable to get p4 for unmapping page=%u", page_number);
  }

  page_table_t* p3 = next_table_address(p4, p4_index(page_number));

  if(p3 == NULL) {
    PANIC("unable to get p3 for unmapping page=%u", page_number);
  }

  page_table_t* p2 = next_table_address(p3, p3_index(page_number));

  if(p2 == NULL) {
    PANIC("unable to get p2 for unmapping page=%u", page_number);
  }

  page_table_t* p1 = next_table_address(p2, p2_index(page_number));

  if(p1 == NULL) {
    PANIC("unable to get p1 for unmapping page=%u", page_number);
  }

  page_entry_t entry = p1->entries[p1_index(page_number)];

  opt_uint64_t frame_number = pointed_frame(entry);

  if(!frame_number.has_value) {
    PANIC("invalid frame number while unmapping page=%u", page_number);
  }

  memset(&p1->entries[p1_index(page_number)], 0, sizeof(page_entry_t));
  MMU_DEBUG_PAGE_ENTRY("cleared", p1->entries[p1_index(page_number)]);

  // TODO(william): free p(1,2,3) table if empty

  frame_deallocate(frame_number.value);

  // flush the translation lookaside buffer
  // cf. http://os.phil-opp.com/modifying-page-tables.html#unmap
  __asm__("invlpg (%0)" : /* no output */ : "r"(addr) : "memory");

  MMU_DEBUG("unapped page=%u addr=%p", page_number, addr);
}

void map(uint64_t page_number, uint64_t flags)
{
  uint64_t frame = frame_allocate();

  if (frame == 0) {
    PANIC("out of memory");
  }

  map_page_to_frame(page_number, frame, flags);
}

void map_multiple(uint64_t start_page_number,
                  uint32_t number_of_pages,
                  uint64_t flags)
{
  for (uint32_t i = 0; i < number_of_pages; i++) {
    map(start_page_number + i, flags);
  }
}

void unmap_multiple(uint64_t start_page_number, uint32_t number_of_pages)
{
  for (uint32_t i = 0; i < number_of_pages; i++) {
    unmap(start_page_number + i);
  }
}

opt_uint32_t paging_amount_for_byte_size(uint64_t start_address, uint64_t byte_size)
{
  opt_uint32_t out_value;
  out_value.has_value = true;

  opt_uint64_t start_page = page_containing_address(start_address);
  opt_uint64_t end_page = page_containing_address(start_address + byte_size);

  if(!start_page.has_value || !end_page.has_value) {
    out_value.has_value = false;

    return out_value;
  }

  uint32_t difference = (uint32_t)(end_page.value - start_page.value);

  if (difference == 0) {
    out_value.value = 1;

    return out_value;
  }

  out_value.value = difference;

  return out_value;
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
