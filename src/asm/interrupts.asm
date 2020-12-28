%macro def_exception_handler 1
  global exc%1
  exc%1:
    cli
    push dword 0
    push dword %1
    jmp int_common_stub
%endmacro

; See: https://github.com/littleosbook/littleosbook/blob/e90faeb24c5c9fed8cde9a35974893706e81cbbf/interrupts.md
;
;     +------------+
; +40 | %SS        |
; +32 | %RSP       |
; +24 | %CPU FLAGS |
; +16 | %CS        |
;  +8 | %IP        |
;   0 | ERROR CODE | <-- %RSP
;     +------------+
;
%macro def_exception_handler_err 1
  global exc%1
  exc%1:
    cli
    push dword %1
    jmp int_common_stub
%endmacro

%macro def_irq_handler 1
  global irq%1
  irq%1:
    cli
    push dword 0
    push dword (32 + %1)
    jmp irq_common_stub
%endmacro

%macro def_int_handler 1
  global int%1
  int%1:
    cli
    push dword 0
    push dword %1
    jmp int_common_stub
%endmacro

int_common_stub:
  ; save registers
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  ; call handler
  extern int_handler
  call int_handler

  ; restore registers
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  add rsp, 16
  sti
  iretq

irq_common_stub:
  ; save registers
  push rax
  push rbx
  push rcx
  push rdx
  push rsi
  push rdi
  push rbp
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  ; call handler
  extern irq_handler
  call irq_handler

  ; restore registers
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rbp
  pop rdi
  pop rsi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  add rsp, 16
  sti
  iretq

; define exceptions
; should be keep in sync with src/core/isr.h
def_exception_handler 0
def_exception_handler 1
def_exception_handler 2
def_exception_handler 3
def_exception_handler 4
def_exception_handler 5
def_exception_handler 6
def_exception_handler 7
def_exception_handler_err 8
def_exception_handler 9
def_exception_handler_err 10
def_exception_handler_err 11
def_exception_handler_err 12
def_exception_handler_err 13
def_exception_handler_err 14
def_exception_handler 15
def_exception_handler 16
def_exception_handler_err 17
def_exception_handler 18
def_exception_handler 19
def_exception_handler 20
def_exception_handler 21
def_exception_handler 22
def_exception_handler 23
def_exception_handler 24
def_exception_handler 25
def_exception_handler 26
def_exception_handler 27
def_exception_handler 28
def_exception_handler 29
def_exception_handler 30
def_exception_handler 31

; define hardware interrupts
; should be keep in sync with src/core/isr.h
def_irq_handler 0
def_irq_handler 1
def_irq_handler 2
def_irq_handler 3
def_irq_handler 4
def_irq_handler 5
def_irq_handler 6
def_irq_handler 7
def_irq_handler 8
def_irq_handler 9
def_irq_handler 10
def_irq_handler 11
def_irq_handler 12

; define software interrupts
; That is for syscalls
def_int_handler 0x80
