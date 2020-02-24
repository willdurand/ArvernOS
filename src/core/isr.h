/** @file */
#ifndef CORE_ISR_H
#define CORE_ISR_H

#include <stdint.h>

#define PIC1      0x20 // Master PIC
#define PIC2      0xA0 // Slave PIC
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI   0x20 // end of interrupt
#define IRQ_BASE  0x20

// exceptions, cf. http://wiki.osdev.org/Exceptions
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

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36

/// This is the ID of the special interrupt for syscalls.
#define SYSCALL 0x80

/// If this flag is set, the page fault was caused by a page-protection
/// violation, else the page fault was caused by a not-present page.
#define PF_PROTECTION_VIOLATION 1 << 0
/// If this flag is set, the memory access that caused the page fault was a
/// write. Else the access that caused the page fault is a memory read. This
/// bit does not necessarily indicate the cause of the page fault was a read or
/// write violation.
#define PF_CAUSED_BY_WRITE 1 << 1
/// If this flag is set, an access in user mode (CPL=3) caused the page fault.
/// Else an access in supervisor mode (CPL=0, 1, or 2) caused the page fault.
/// This bit does not necessarily indicate the cause of the page fault was a
/// privilege violation.
#define PF_USER_MODE 1 << 2
/// If this flag is set, the page fault is a result of the processor reading a
/// 1 from a reserved field within a page-translation-table entry.
#define PF_MALFORMED_TABLE 1 << 3
/// If this flag is set, it indicates that the access that caused the page
/// fault was an instruction fetch.
#define PF_INSTRUCTION_FETCH 1 << 4

// These functions are declared in the `interrupts.asm` file.
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();

extern void isr0x80();

typedef struct registers
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
} __attribute__((packed)) registers_t;

typedef struct stack
{
  uint64_t instruction_pointer;
  uint64_t code_segment;
  uint64_t cpu_flags;
  uint64_t stack_pointer;
  uint64_t stack_segment;
} __attribute__((packed)) stack_t;

/// This type represents an interrupt handler.
typedef void (*isr_t)(stack_t* stack);

/**
 * Initializes the interrupt service routine.
 */
void isr_init();

/**
 * Enables hardware interrupts.
 */
void irq_init();

/**
 * Disables hardware interrupts.
 */
void irq_disable();

/**
 * This is the handler for software interrupts.
 *
 * @param id the interrupt id
 * @param stack the stack
 */
void isr_handler(uint64_t id, uint64_t stack) __asm__("isr_handler");

/**
 * This is the handler for harware interrupts.
 *
 * @param id the interrupt id
 * @param stack the stack
 */
void irq_handler(uint64_t id, uint64_t stack) __asm__("irq_handler");

/**
 * Registers a handler for a given interrupt.
 *
 * @param id an interrupt id
 * @param handler the handler to attach to the interrupt
 */
void isr_register_handler(uint64_t id, isr_t handler);

#endif
