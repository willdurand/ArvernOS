/**
 * @file
 * @see
 * https://github.com/xen-project/xen/blob/master/xen/include/public/arch-x86/hvm/start_info.h
 *
 * This module manages the PVH boot information, which is passed by the
 * bootloader in `ebx` when the kernel is loaded using the PVH ELF note (see
 * `asm/notes.asm`).
 */
#ifndef CORE_PVH_H
#define CORE_PVH_H

#include <boot.h>
#include <stdint.h>

// The structures below come from the Xen public headers and are MIT-licensed.
//
// SPDX-License-Identifier: MIT
// Copyright (c) 2016, Citrix Systems, Inc.
typedef struct hvm_start_info
{
  uint32_t magic;
  uint32_t version;
  uint32_t flags;
  uint32_t nr_modules;
  uint64_t modlist_paddr;
  uint64_t cmdline_paddr;
  uint64_t rsdp_paddr;
  uint64_t memmap_paddr;
  uint32_t memmap_entries;
  uint32_t reserved;
} hvm_start_info_t;

struct hvm_memmap_table_entry
{
  uint64_t addr;
  uint64_t size;
  uint32_t type;
  uint32_t reserved;
};

struct hvm_modlist_entry
{
  uint64_t paddr;
  uint64_t size;
  uint64_t cmdline_paddr;
  uint32_t reserved;
};

/// The physical memory map, as an array of `hvm_memmap_table_entry` (whose
/// layout matches `memory_region_t`) followed by its number of entries.
typedef struct pvh_mmap
{
  uint64_t addr;
  uint32_t count;
} pvh_mmap_t;

/**
 * Reads the PVH boot information.
 *
 * @param info the PVH boot information
 */
void pvh_init(hvm_start_info_t* info);

/**
 * Returns the command line, if any.
 *
 * @return a pointer to the command line or `0`
 */
const char* pvh_get_cmdline();

/**
 * Returns the physical memory map reported by the bootloader.
 *
 * @return a pointer to the physical memory map
 */
pvh_mmap_t* pvh_get_mmap();

/**
 * Returns the physical address range spanned by all boot modules (e.g. the
 * init ramdisk), if any.
 *
 * @param start set to the start address of the range, or `0` if there is no
 *   boot module
 * @param end set to the end address (exclusive) of the range, or `0` if there
 *   is no boot module
 */
void pvh_get_modules_range(uintptr_t* start, uintptr_t* end);

/**
 * Fills the boot information from the PVH boot information.
 *
 * @param info the boot information to fill
 */
void pvh_get_boot_info(boot_info_t* info);

#endif
