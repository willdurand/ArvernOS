global switch_to_usermode

switch_to_usermode:
  mov rcx, rdi    ; new instruction pointer
  mov rsp, rsi    ; new stack pointer
  mov r11, 0x0202 ; cpu flags
  o64 sysret
