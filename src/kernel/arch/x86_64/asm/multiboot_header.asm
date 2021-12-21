// This is a Multiboot-compliant header file in assembly code, based on Philipp
// Oppermann's OS [1], which was released under the MIT License - Copyright (c)
// 2019 Philipp Oppermann.
//
// [1]: https://os.phil-opp.com/edition-1/

.intel_syntax noprefix

.section ".multiboot_header"
.align 8

.equ MULTIBOOT2_MAGIC_NUMBER, 0xe85250d6
.equ PROTECTED_MODE_CODE,     0 // architecture 0 (protected mode i386)
                                // architecture 4 (MIPS)

header_start:
  // long = double word
  .long MULTIBOOT2_MAGIC_NUMBER   // magic number
  .long PROTECTED_MODE_CODE       // architecture
  .long header_end - header_start // header length

  // checksum
  .long 0x100000000 - (MULTIBOOT2_MAGIC_NUMBER + 0 + (header_end - header_start))

.ifdef ENABLE_FRAMEBUFFER
  // framebuffer
  .word 5
  .word 0
  .long 20
  .long VBE_WIDTH
  .long VBE_HEIGHT
  .long VBE_BPP
  .long 0
.endif

  // required end tag
  // word = 16 bits on x86_64
  .word 0  // type
  .word 0  // flags
  .long 8  // size
header_end:
