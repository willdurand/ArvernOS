.section ".text.boot"

.global start

start:
  // stop secondary cores
  mrc p15, 0, r5, c0, c0, 5
  and r5, r5, #3
  cmp r5, #0
  bne hang

  // set up the stack
  ldr r5, =start
  mov sp, r5

  // enable FPU
  mrc p15, 0, r5, c1, c0, 2
  orr r5, r5, #(0xf << 20)
  mcr p15, 0, r5, c1, c0, 2
  isb
  mov r5, #0x40000000
  vmsr fpexc, r5

  // clear bss section
  ldr r4, =__bss_start
  ldr r9, =__bss_end
  mov r5, #0
  mov r6, #0
  mov r7, #0
  mov r8, #0
  b clear_bss

zero_bss:
  stmia r4!, {r5-r8}

clear_bss:
  cmp r4, r9
  blo zero_bss

  bl kmain
  b hang

hang:
  wfe
  b hang
