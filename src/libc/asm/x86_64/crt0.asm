.intel_syntax noprefix

.section ".text"

.global _start

_start:
  // Prepare the arguments for `start_main()`.
  mov rdi, offset main // `main()`
  pop rsi              // `argc`
  pop rdx              // `argv`
  call start_main
