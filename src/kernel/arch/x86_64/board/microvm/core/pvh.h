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

// The `hvm_start_info` structure below comes from the Xen public headers and
// is MIT-licensed.
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

#endif
