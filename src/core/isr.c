#include <stdlib.h>
#include <drivers/screen.h>
#include "isr.h"

char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_init()
{
    set_idt_gate(0, (uint64_t) isr0);
    set_idt_gate(1, (uint64_t) isr1);
    set_idt_gate(2, (uint64_t) isr2);
    set_idt_gate(3, (uint64_t) isr3);
    set_idt_gate(4, (uint64_t) isr4);
    set_idt_gate(5, (uint64_t) isr5);
    set_idt_gate(6, (uint64_t) isr6);
    set_idt_gate(7, (uint64_t) isr7);
    set_idt_gate(8, (uint64_t) isr8);
    set_idt_gate(9, (uint64_t) isr9);
    set_idt_gate(10, (uint64_t) isr10);
    set_idt_gate(11, (uint64_t) isr11);
    set_idt_gate(12, (uint64_t) isr12);
    set_idt_gate(13, (uint64_t) isr13);
    set_idt_gate(14, (uint64_t) isr14);
    set_idt_gate(15, (uint64_t) isr15);
    set_idt_gate(16, (uint64_t) isr16);
    set_idt_gate(17, (uint64_t) isr17);
    set_idt_gate(18, (uint64_t) isr18);
    set_idt_gate(19, (uint64_t) isr19);
    set_idt_gate(20, (uint64_t) isr20);
    set_idt_gate(21, (uint64_t) isr21);
    set_idt_gate(22, (uint64_t) isr22);
    set_idt_gate(23, (uint64_t) isr23);
    set_idt_gate(24, (uint64_t) isr24);
    set_idt_gate(25, (uint64_t) isr25);
    set_idt_gate(26, (uint64_t) isr26);
    set_idt_gate(27, (uint64_t) isr27);
    set_idt_gate(28, (uint64_t) isr28);
    set_idt_gate(29, (uint64_t) isr29);
    set_idt_gate(30, (uint64_t) isr30);
    set_idt_gate(31, (uint64_t) isr31);

    set_idt();
}

void isr_handler(registers_t r)
{
    char s[3];

    screen_print("Received interrupt: ");
    //itoa(r.int_no, s);
    //screen_print(s);
    screen_print("\n");
    screen_print(exception_messages[r.int_no]);
    screen_print("\n");
}
