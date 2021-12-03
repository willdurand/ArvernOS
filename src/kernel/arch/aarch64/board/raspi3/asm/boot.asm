// At an early stage, this file was mainly inspired by [the work][1] of Zoltan
// Baldaszti (@bztsrc) licensed under the MIT License: Copyright (C) 2017 bzt
// (bztsrc@github).
//
// [1]: https://github.com/bztsrc/raspi3-tutorial

#include <core/register.h>

.section ".text.boot"

.global start

start:
  // backup w0, which contains the atags/device tree address
  mov w6, w0

  // read cpu id, stop secondary cores
  mrs x0, mpidr_el1
  and x0, x0, #0xFF
  cbz x0, setup_primary
  b hang

setup_primary:
  // disable coprocessor traps
  mov x0, #0x33FF
  msr cptr_el2, x0
  msr hstr_el2, xzr
  // enable FP/SIMD at EL1
  mov x0, #(3 << 20)
  msr cpacr_el1, x0

  ldr x0, =SCTLR_VALUE_MMU_DISABLED
  msr sctlr_el1, x0

  // configure aarch64 in EL1
  mov x0, #(1 << 31)
  msr hcr_el2, x0

  // change execution level to EL1
  ldr x0, =SPSR_VALUE
  msr spsr_el2, x0
  ldr x0, =el1_start
  msr elr_el2, x0
  eret

el1_start:
  // set top of stack just before our code (stack grows to a lower address per
  // AAPCS64)
  ldr x1, =start
  mov sp, x1

  // load vector table
  ldr x2, =vectors
  msr vbar_el1, x2

  // clear bss
  ldr x5, =__bss_start
  ldr w2, =__bss_size

clear_bss:
  cbz w2, call_kmain
  str xzr, [x5], #8
  sub w2, w2, #1
  cbnz w2, clear_bss

call_kmain:
  // pass w0 (previously stored in w6 at the top of the file), which contains
  // the atags/device tree address
  mov w0, w6
  bl kmain
  b hang

hang:
  wfe
  b hang
