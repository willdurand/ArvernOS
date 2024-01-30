#include "paging.h"
#include <arvern/utils.h>
#include <assert.h>
#include <core/multiboot.h>
#include <core/register.h>
#include <inttypes.h>
#include <mmu/bitmap.h>
#include <mmu/logging.h>
#include <panic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MMU_DEBUG_PAGE_ENTRY(message, e)                                       \
  {                                                                            \
    MMU_DEBUG("%s page entry addr=%p"                                          \
              " present=%d"                                                    \
              " writable=%d"                                                   \
              " no_execute=%d"                                                 \
              " user_accessible=%d"                                            \
              " huge_page=%d",                                                 \
              message,                                                         \
              (e & 0x000ffffffffff000),                                        \
              (e & PAGING_FLAG_PRESENT) == PAGING_FLAG_PRESENT,                \
              (e & PAGING_FLAG_WRITABLE) == PAGING_FLAG_WRITABLE,              \
              (e & PAGING_FLAG_NO_EXECUTE) == PAGING_FLAG_NO_EXECUTE,          \
              (e & PAGING_FLAG_USER_ACCESSIBLE) ==                             \
                PAGING_FLAG_USER_ACCESSIBLE,                                   \
              (e & PAGING_FLAG_HUGE_PAGE) == PAGING_FLAG_HUGE_PAGE);           \
  }

void map_page_to_frame(page_number_t page_number,
                       uint64_t frame,
                       uint64_t flags);
void enable_nxe_bit();
void enable_write_protection();
void remap_kernel();
void zero_table(page_table_t* table);
page_table_t* next_table_address(page_table_t* table, uint64_t index);
opt_uint64_t pointed_frame(uint64_t entry);
page_table_t* next_table_create(page_table_t* table, uint64_t index);
page_table_t* get_p4();
opt_uint64_t paging_frame_allocate();
void paging_frame_deallocate(frame_number_t frame_number);

extern uint8_t frames_for_bitmap;
extern bitmap_t* allocated_frames;
extern void load_p4(uint64_t addr);

static page_table_t* active_p4_table = (page_table_t*)P4_TABLE;
static bool can_deallocate_frames = false;

void paging_init()
{
  INFO("%s", "mmu: initialize paging");

  enable_nxe_bit();
  enable_write_protection();
  remap_kernel();
}

void remap_kernel()
{
  uint64_t inactive_page_table_frame = paging_frame_allocate().value;
  page_number_t temporary_page = 0xcafec000;

  map_page_to_frame(temporary_page,
                    inactive_page_table_frame,
                    PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

  page_table_t* inactive_page_table =
    (page_table_t*)page_start_address(temporary_page);
  // now we are able to zero the table
  zero_table(inactive_page_table);
  // set up recursive mapping for the table
  inactive_page_table->entries[511] =
    inactive_page_table_frame | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;
  unmap(temporary_page);

  uint64_t backup_addr = read_cr3();
  MMU_DEBUG("backup_addr=%p backup_frame=%" PRIu64,
            backup_addr,
            frame_containing_address(backup_addr));

  // map temporary_page to current p4 table
  map_page_to_frame(
    temporary_page, backup_addr, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);

  page_table_t* p4_table = (page_table_t*)page_start_address(temporary_page);

  // overwrite recursive mapping
  active_p4_table->entries[511] =
    inactive_page_table_frame | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;
  write_cr3(read_cr3());

  // We shouldn't deallocate the `inactive_page_table_frame`.
  can_deallocate_frames = true;

  // Identity map first 1 MB.
  // See: https://wiki.osdev.org/Memory_Map_(x86)
  for (uint64_t addr = 0; addr < 0x000A0000; addr += PAGE_SIZE) {
    identity_map(addr, PAGING_FLAG_PRESENT);
  }
  for (uint64_t addr = 0x000A0000; addr < 0x00100000; addr += PAGE_SIZE) {
    identity_map(addr, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
  }
  MMU_DEBUG("%s", "mapped first 1MB!");

  multiboot_tag_framebuffer_t* entry = multiboot_get_framebuffer();
  if (entry->common.framebuffer_addr != 0xB8000) {
    uint64_t pages_for_fb =
      (entry->common.framebuffer_pitch * entry->common.framebuffer_height) /
      PAGE_SIZE;

    for (uint64_t i = 0; i < pages_for_fb; i++) {
      identity_map((uint64_t)entry->common.framebuffer_addr + (i * PAGE_SIZE),
                   PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
    }
    MMU_DEBUG("%s", "mapped VBE framebuffer!");
  }

  MMU_DEBUG("%s", "mapping kernel sections");
  multiboot_tag_elf_sections_t* tag =
    (multiboot_tag_elf_sections_t*)multiboot_find(
      MULTIBOOT_TAG_TYPE_ELF_SECTIONS);

  uint64_t i = 0;
  multiboot_elf_sections_entry_t* elf = NULL;

  // TODO: create a foreach macro
  for (i = 0, elf = ((multiboot_tag_elf_sections_t*)tag)->sections;
       i < ((multiboot_tag_elf_sections_t*)tag)->num;
       elf =
         (multiboot_elf_sections_entry_t*)((uint64_t)elf +
                                           ((multiboot_tag_elf_sections_t*)tag)
                                             ->section_size),
      i++) {
    if ((elf->flags & MULTIBOOT_ELF_SECTION_FLAG_ALLOCATED) !=
          MULTIBOOT_ELF_SECTION_FLAG_ALLOCATED ||
        elf->type == MULTIBOOT_ELF_SECTION_TYPE_NULL) {
      continue;
    }

    frame_number_t start_frame_number = frame_containing_address(elf->addr);
    frame_number_t end_frame_number =
      frame_containing_address(elf->addr + elf->size - 1);

    uint64_t flags = PAGING_FLAG_PRESENT;

    if (elf->flags & MULTIBOOT_ELF_SECTION_FLAG_WRITABLE) {
      flags |= PAGING_FLAG_WRITABLE;
    }

    if (!(elf->flags & MULTIBOOT_ELF_SECTION_FLAG_EXECUTABLE)) {
      flags |= PAGING_FLAG_NO_EXECUTE;
    }

    for (uint64_t i = start_frame_number; i <= end_frame_number; i++) {
      identity_map(frame_start_address(i), flags);
    }
  }
  MMU_DEBUG("%s", "kernel sections mapped!");

  // Once the kernel is correctly mapped with the right flags, we map the rest
  // of the reserved area that was determined by the multiboot code. We make
  // sure that it is PRESENT, we probably don't need any other flags.
  reserved_areas_t reserved = multiboot_get_reserved_areas();

  frame_number_t multiboot_start_frame =
    frame_containing_address(reserved.kernel_end);
  frame_number_t multiboot_end_frame =
    frame_containing_address(reserved.end - 1);

  MMU_DEBUG("mapping end of the reserved area: start_frame=%" PRIu64
            " end_frame=%" PRIu64,
            multiboot_start_frame,
            multiboot_end_frame);

  for (uint64_t i = multiboot_start_frame; i <= multiboot_end_frame; i++) {
    identity_map(frame_start_address(i), PAGING_FLAG_PRESENT);
  }
  MMU_DEBUG("%s", "end of reserved area mapped!");

  // Bitmap for allocated frames.
  MMU_DEBUG("mapping %d pages for frame allocator, starting at addr=%p",
            frames_for_bitmap,
            allocated_frames);
  for (uint8_t i = 0; i < frames_for_bitmap; i++) {
    identity_map((uint64_t)allocated_frames + (i * PAGE_SIZE),
                 PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
  }
  MMU_DEBUG("%s", "pages for frame allocator mapped!");

  // restore recursive mapping to original p4 table
  p4_table->entries[511] =
    backup_addr | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;
  write_cr3(read_cr3());
  MMU_DEBUG("%s", "restored recursive mapping");

  unmap(temporary_page);

  write_cr3(inactive_page_table_frame);
  DEBUG("switched to new table: cr3=%p", (void*)read_cr3());

  uint64_t old_page_table_frame =
    frame_start_address(frame_containing_address(backup_addr));
  page_number_t old_page_table = page_containing_address(old_page_table_frame);
  unmap(old_page_table);
  DEBUG("guard page at %p", (void*)page_start_address(old_page_table));
}

void enable_nxe_bit()
{
  uint64_t efer = read_msr(IA32_EFER);
  write_msr(IA32_EFER, efer | (1 << 11));
  DEBUG("enabled NXE bit old_efer=%#x new_efer=%#x", efer, read_msr(IA32_EFER));
}

void enable_write_protection()
{
  write_cr0(read_cr0() | (1 << 16));
  DEBUG("enabled write protection: cr0=%#x", read_cr0());
}

void zero_table(page_table_t* table)
{
  memset((void*)table, 0, sizeof(page_table_t));
}

page_table_t* next_table_address(page_table_t* table, uint64_t index)
{
  if ((table->entries[index] & PAGING_FLAG_PRESENT) != PAGING_FLAG_PRESENT) {
    MMU_DEBUG("entry at index=%" PRIu64 " in table=%p is not present, "
              "returning NULL",
              index,
              table);
    return NULL;
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

opt_uint64_t translate(uint64_t virtual_address)
{
  uint64_t offset = virtual_address % PAGE_SIZE;
  page_number_t page = page_containing_address(virtual_address);
  opt_uint64_t frame = translate_page(page);

  if (!frame.has_value) {
    return frame;
  }

  return (opt_uint64_t){ .has_value = true,
                         .value = frame.value * PAGE_SIZE + offset };
}

page_number_t page_containing_address(uint64_t virtual_address)
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

uint64_t page_start_address(page_number_t page_number)
{
  return page_number * PAGE_SIZE;
}

opt_uint64_t translate_page(page_number_t page_number)
{
  page_table_t* p4 = get_p4();

  page_table_t* p3 = next_table_address(p4, _p4_index(page_number));

  if (p3) {
    uint64_t p3_entry = p3->entries[_p3_index(page_number)];
    opt_uint64_t frame = pointed_frame(p3_entry);

    if (frame.has_value &&
        (p3_entry & PAGING_FLAG_HUGE_PAGE) == PAGING_FLAG_HUGE_PAGE) {
      MMU_DEBUG(
        "1GB huge page=%" PRIu64 " frame=%" PRIu64, page_number, frame.value);

      if (frame.value % (PAGE_ENTRIES * PAGE_ENTRIES) == 0) {
        frame.value +=
          _p2_index(page_number) * PAGE_ENTRIES + _p1_index(page_number);

        return frame;
      }

      PANIC("misaligned 1GB page=%" PRIu64, page_number);
    }
  } else {
    MMU_DEBUG("did not find p3 (%p), returning no value", p3);
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  page_table_t* p2 = next_table_address(p3, _p3_index(page_number));

  if (p2) {
    if ((p3->entries[_p3_index(page_number)] & PAGING_FLAG_HUGE_PAGE) ==
        PAGING_FLAG_HUGE_PAGE) {
      opt_uint64_t frame = pointed_frame(p2->entries[_p2_index(page_number)]);

      MMU_DEBUG(
        "2MB huge page=%" PRIu64 " frame=%" PRIu64, page_number, frame.value);

      if (frame.value % PAGE_ENTRIES == 0) {
        frame.value += _p1_index(page_number);

        return frame;
      }

      PANIC("misaligned 2MB page=%" PRIu64, page_number);
    }
  } else {
    MMU_DEBUG("did not find p2 (%p), returning no value", p2);
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  page_table_t* p1 = next_table_address(p2, _p2_index(page_number));

  if (p1 == NULL) {
    MMU_DEBUG("did not find p1 (%p), returning no value", p1);
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  return pointed_frame(p1->entries[_p1_index(page_number)]);
}

uint64_t _p4_index(page_number_t page_number)
{
  return (page_number >> 27) & 0777;
}

uint64_t _p3_index(page_number_t page_number)
{
  return (page_number >> 18) & 0777;
}

uint64_t _p2_index(page_number_t page_number)
{
  return (page_number >> 9) & 0777;
}

uint64_t _p1_index(page_number_t page_number)
{
  return (page_number >> 0) & 0777;
}

opt_uint64_t pointed_frame(uint64_t entry)
{
  if (entry & PAGING_FLAG_PRESENT) {
    return (opt_uint64_t){ .has_value = true,
                           .value = frame_containing_address(
                             entry & 0x000ffffffffff000) };
  }

  return (opt_uint64_t){ .has_value = false, .value = 0 };
}

/**
 * Maps a page (number) to a frame (physical address).
 *
 * @param page_number a page number (not an address)
 * @param frame a physical address (frame)
 * @param flags paging flags
 */
void map_page_to_frame(page_number_t page_number,
                       uint64_t frame,
                       uint64_t flags)
{
  MMU_DEBUG("mapping page=%" PRIu64 " (start_addr=%p) to frame=%" PRIu64
            " (start_addr=%p) with flags=%#x",
            page_number,
            page_start_address(page_number),
            frame_containing_address(frame),
            frame,
            flags);

#ifndef TEST_ENV
  // Do not call `translate_page()` here, otherwise our test suite wouldn't be
  // able to fake the calls to `next_table_address()` right after.
  // opt_uint64_t maybe_frame = translate_page(page_number);
  // assert(!maybe_frame.has_value);
#endif

  uint64_t p4_idx = _p4_index(page_number);
  uint64_t p3_idx = _p3_index(page_number);
  uint64_t p2_idx = _p2_index(page_number);
  uint64_t p1_idx = _p1_index(page_number);

  MMU_DEBUG("p1_idx=%" PRIu64 " p2_idx=%" PRIu64 " p3_idx=%" PRIu64
            " p4_idx=%" PRIu64,
            p1_idx,
            p2_idx,
            p3_idx,
            p4_idx);

  page_table_t* p4 = get_p4();
  assert(p4 != NULL);

  page_table_t* p3 = next_table_create(p4, p4_idx);
  assert(p3 != NULL);

  page_table_t* p2 = next_table_create(p3, p3_idx);
  assert(p2 != NULL);

  page_table_t* p1 = next_table_create(p2, p2_idx);
  assert(p1 != NULL);

#ifndef CONFIG_LINUX_COMPAT
  // TODO: We should not have to skip this assertion when CONFIG_LINUX_COMPAT
  // is set.
  assert((p1->entries[p1_idx] & PAGING_FLAG_PRESENT) == 0);
#endif

  p1->entries[p1_idx] = frame | flags;

  MMU_DEBUG("mapped page=%" PRIu64 " to frame=%" PRIu64 " (addr=%p)",
            page_number,
            frame_containing_address(frame),
            frame);
}

page_table_t* next_table_create(page_table_t* table, uint64_t index)
{
  MMU_DEBUG("table=%p index=%" PRIu64, (void*)table, index);

  if (table &&
      (table->entries[index] & PAGING_FLAG_PRESENT) == PAGING_FLAG_PRESENT) {
    MMU_DEBUG("page entry at index=%" PRIu64 " is present", index);

    return next_table_address(table, index);
  }

  MMU_DEBUG("page entry at index=%" PRIu64 " is not present, creating entry",
            index);

  opt_uint64_t frame = paging_frame_allocate();
  assert(frame.has_value);

  table->entries[index] =
    frame.value | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;

  page_table_t* next_table = next_table_address(table, index);

  zero_table(next_table);

  return next_table;
}

page_table_t* get_p4()
{
  return active_p4_table;
}

void _set_p4(page_table_t* table)
{
  MMU_DEBUG("setting p4 to addr=%p", table);

  active_p4_table = table;
}

void unmap(page_number_t page_number)
{
  uint64_t addr = page_start_address(page_number);

  MMU_DEBUG("unmapping page_number=%" PRIu64 " addr=%p", page_number, addr);

#ifndef TEST_ENV
  // Do not call `translate()` here, otherwise our test suite wouldn't be able
  // to fake the calls to `next_table_address()` right after.
  if (!translate(addr).has_value) {
    MMU_DEBUG("cannot unmap page=%" PRIu64 " (start_address=%p) because it is "
              "not mapped",
              page_number,
              addr);
    return;
  }
#endif

  page_table_t* p4 = get_p4();
  assert(p4 != NULL);
  page_table_t* p3 = next_table_address(p4, _p4_index(page_number));
  assert(p3 != NULL);
  page_table_t* p2 = next_table_address(p3, _p3_index(page_number));
  assert(p2 != NULL);
  page_table_t* p1 = next_table_address(p2, _p2_index(page_number));
  assert(p1 != NULL);

  uint64_t entry = p1->entries[_p1_index(page_number)];

  opt_uint64_t frame = pointed_frame(entry);
  assert(frame.has_value);

  frame_number_t frame_number = frame.value;

  p1->entries[_p1_index(page_number)] = 0;

#ifndef TEST_ENV
  // flush the translation lookaside buffer
  // cf. http://os.phil-opp.com/modifying-page-tables.html#unmap
  __asm__("invlpg (%0)"
          : /* no output */
          : "r"(addr)
          : "memory");
#endif

  // TODO: Free p(1,2,3) table if empty.

#ifdef TEST_ENV
  can_deallocate_frames = true;
#endif

  if (can_deallocate_frames) {
    paging_frame_deallocate(frame_number);
  }

  MMU_DEBUG("unmapped page=%" PRIu64 " addr=%p", page_number, addr);
}

void map(page_number_t page_number, uint64_t flags)
{
  opt_uint64_t frame = paging_frame_allocate();

  if (!frame.has_value) {
    PANIC("out of memory");
  }

  map_page_to_frame(page_number, frame.value, flags);
}

void map_multiple(uint64_t start_page_number,
                  uint32_t number_of_pages,
                  uint64_t flags)
{
  MMU_DEBUG("mapping %ld pages, starting at page=%" PRIu64,
            number_of_pages,
            start_page_number);

  for (uint32_t i = 0; i < number_of_pages; i++) {
    map(start_page_number + i, flags);
  }
}

void unmap_multiple(page_number_t start_page_number, uint32_t number_of_pages)
{
  MMU_DEBUG("unmapping %ld pages, starting at page=%" PRIu64,
            number_of_pages,
            start_page_number);

  for (uint32_t i = 0; i < number_of_pages; i++) {
    unmap(start_page_number + i);
  }
}

uint32_t paging_amount_for_byte_size(uint64_t start_address, uint64_t byte_size)
{
  page_number_t start_page = page_containing_address(start_address);
  page_number_t end_page = page_containing_address(start_address + byte_size);

  uint32_t difference = (uint32_t)(end_page - start_page) + 1;

  return difference;
}

opt_uint64_t paging_frame_allocate()
{
#ifdef TEST_ENV
  return test_frame_allocate();
#endif

  return frame_allocate();
}

void paging_frame_deallocate(frame_number_t frame_number)
{
#ifdef TEST_ENV
  test_frame_deallocate(frame_number);
#else
  frame_deallocate(frame_number);
#endif
}

void identity_map(uint64_t address, uint64_t flags)
{
  page_number_t page = page_containing_address(address);
  map_page_to_frame(page, address, flags);
}
