/** @file */
#ifndef CORE_INTERRUPT_H
#define CORE_INTERRUPT_H

#include <stdint.h>

#define PIC1      0x20 // Primary PIC
#define PIC2      0xA0 // Secondary PIC
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI   0x20 // end of interrupt
#define IRQ_BASE  0x20

// Exceptions, cf. http://wiki.osdev.org/Exceptions
#define EXCEPTION_DE 0
#define EXCEPTION_DB 1
#define EXCEPTION_BP 3
#define EXCEPTION_OF 4
#define EXCEPTION_BR 5
#define EXCEPTION_UD 6
#define EXCEPTION_NM 7
#define EXCEPTION_DF 8
#define EXCEPTION_TS 10
#define EXCEPTION_NP 11
#define EXCEPTION_SS 12
#define EXCEPTION_GP 13
#define EXCEPTION_PF 14
// 15 is "reserved"
#define EXCEPTION_MF 16
#define EXCEPTION_AC 17
// ...

// Hardware interrupts
#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44

// These functions are declared in `src/arch/x86_64/isr.asm`.
extern void exc0();
extern void exc1();
extern void exc2();
extern void exc3();
extern void exc4();
extern void exc5();
extern void exc6();
extern void exc7();
extern void exc8();
extern void exc9();
extern void exc10();
extern void exc11();
extern void exc12();
extern void exc13();
extern void exc14();
extern void exc15();
extern void exc16();
extern void exc17();
extern void exc18();
extern void exc19();
extern void exc20();
extern void exc21();
extern void exc22();
extern void exc23();
extern void exc24();
extern void exc25();
extern void exc26();
extern void exc27();
extern void exc28();
extern void exc29();
extern void exc30();
extern void exc31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();

/// This structure represents the interrupt stack.
typedef struct isr_stack
{
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rbp;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;
  uint64_t id;
  uint64_t error_code;
  uint64_t instruction_pointer;
  uint64_t code_segment;
  uint64_t cpu_flags;
  uint64_t stack_pointer;
  uint64_t stack_segment;
} __attribute__((packed)) isr_stack_t;

#endif
