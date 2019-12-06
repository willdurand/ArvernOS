#ifndef CORE_ISR_H
#define CORE_ISR_H

#include <core/idt.h>
#include <stdint.h>

#define PIC1        0x20 // Master PIC
#define PIC2        0xA0 // Slave PIC
#define PIC1_DATA   (PIC1 + 1)
#define PIC2_DATA   (PIC2 + 1)
#define PIC_EOI     0x20 // end of interrupt
#define IRQ_BASE    0x20

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

// These functions are declared in interrupts.asm file
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

typedef struct registers {
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

typedef struct stack {
    uint64_t instruction_pointer;
    uint64_t code_segment;
    uint64_t cpu_flags;
    uint64_t stack_pointer;
    uint64_t stack_segment;
} __attribute__((packed)) stack_t;

typedef void (*isr_t)(stack_t* stack);

void isr_init();
void irq_init();
void irq_disable();
void isr_handler(uint64_t id, uint64_t stack) __asm__("isr_handler");
void irq_handler(uint64_t id, uint64_t stack) __asm__("irq_handler");
void register_interrupt_handler(uint64_t id, isr_t handler);

// That is for syscalls
#define SYSCALL 0x80
extern void isr0x80();

#endif
