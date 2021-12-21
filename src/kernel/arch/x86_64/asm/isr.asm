.intel_syntax noprefix

// The code samples from "The little book about OS development" - which is where
// the macros in this file are coming from - are in the public domain.
.macro def_exception_handler, id
  .global "exc\id"
  exc\id:
    cli
    push 0
    push \id
    jmp int_common_stub
.endm

// See: https://github.com/littleosbook/littleosbook/blob/e90faeb24c5c9fed8cde9a35974893706e81cbbf/interrupts.md
//
//     +------------+
// +40 | %SS        |
// +32 | %RSP       |
// +24 | %CPU FLAGS |
// +16 | %CS        |
//  +8 | %IP        |
//   0 | ERROR CODE | <-- %RSP
//     +------------+
//
.macro def_exception_handler_err, id
  .global "exc\id"
  exc\id:
    cli
    push \id
    jmp int_common_stub
.endm

.macro def_irq_handler, id
  .global "irq\id"
  irq\id:
    cli
    push 0
    push (32 + \id)
    jmp irq_common_stub
.endm

.macro def_int_handler, id
  .global "int\id"
  int\id:
    cli
    push 0
    push \id
    jmp int_common_stub
.endm

.macro save_registers
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
.endm

.macro restore_registers
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
.endm

int_common_stub:
  save_registers
  call isr_int_handler
  restore_registers
  add rsp, 16
  sti
  iretq

irq_common_stub:
  save_registers
  call isr_irq_handler
  restore_registers
  add rsp, 16
  sti
  iretq

// define exceptions
// should be keep in sync with src/arch/x86_64/core/isr.h
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

// define hardware interrupts
// should be keep in sync with src/arch/x86_64/core/isr.h
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
