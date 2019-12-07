#include "isr.h"
#include <core/debug.h>
#include <core/ports.h>
#include <core/syscall.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <stdlib.h>

stack_t* get_stack(uint64_t id, uint64_t stack);

void breakpoint_handler(stack_t* stack);
void double_fault_handler(stack_t* stack);

const char* exception_messages[] = {
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

isr_t interrupt_handlers[256];

void isr_init() {
    // start initialization
    port_byte_out(PIC1, 0x11);
    port_byte_out(PIC2, 0x11);

    // set IRQ base numbers for each PIC
    port_byte_out(PIC1_DATA, IRQ_BASE);
    port_byte_out(PIC2_DATA, IRQ_BASE + 8);

    // use IRQ number 2 to relay IRQs from the slave PIC
    port_byte_out(PIC1_DATA, 0x04);
    port_byte_out(PIC2_DATA, 0x02);

    // finish initialization
    port_byte_out(PIC1_DATA, 0x01);
    port_byte_out(PIC2_DATA, 0x01);

    // mask all IRQs
    port_byte_out(PIC1_DATA, 0x00);
    port_byte_out(PIC2_DATA, 0x00);

    register_idt_gate(0, (uint64_t) isr0);
    register_idt_gate(1, (uint64_t) isr1);
    register_idt_gate(2, (uint64_t) isr2);
    register_idt_gate(3, (uint64_t) isr3);
    register_idt_gate(4, (uint64_t) isr4);
    register_idt_gate(5, (uint64_t) isr5);
    register_idt_gate(6, (uint64_t) isr6);
    register_idt_gate(7, (uint64_t) isr7);
    register_idt_gate(8, (uint64_t) isr8);
    register_idt_gate(9, (uint64_t) isr9);
    register_idt_gate(10, (uint64_t) isr10);
    register_idt_gate(11, (uint64_t) isr11);
    register_idt_gate(12, (uint64_t) isr12);
    register_idt_gate(13, (uint64_t) isr13);
    register_idt_gate(14, (uint64_t) isr14);
    register_idt_gate(15, (uint64_t) isr15);
    register_idt_gate(16, (uint64_t) isr16);
    register_idt_gate(17, (uint64_t) isr17);
    register_idt_gate(18, (uint64_t) isr18);
    register_idt_gate(19, (uint64_t) isr19);
    register_idt_gate(20, (uint64_t) isr20);
    register_idt_gate(21, (uint64_t) isr21);
    register_idt_gate(22, (uint64_t) isr22);
    register_idt_gate(23, (uint64_t) isr23);
    register_idt_gate(24, (uint64_t) isr24);
    register_idt_gate(25, (uint64_t) isr25);
    register_idt_gate(26, (uint64_t) isr26);
    register_idt_gate(27, (uint64_t) isr27);
    register_idt_gate(28, (uint64_t) isr28);
    register_idt_gate(29, (uint64_t) isr29);
    register_idt_gate(30, (uint64_t) isr30);
    register_idt_gate(31, (uint64_t) isr31);

    register_idt_gate(IRQ0, (uint64_t) irq0);
    register_idt_gate(IRQ1, (uint64_t) irq1);
    register_idt_gate(IRQ2, (uint64_t) irq2);
    register_idt_gate(IRQ3, (uint64_t) irq3);
    register_idt_gate(IRQ4, (uint64_t) irq4);

    // syscalls
    register_idt_gate(SYSCALL, (uint64_t) isr0x80);

    // handlers for isr exceptions
    register_interrupt_handler(EXCEPTION_BP, breakpoint_handler);
    register_interrupt_handler(EXCEPTION_DF, double_fault_handler);

    idt_init();
}

void irq_init() {
    __asm__("sti");
}

void irq_disable() {
    __asm__("cli");
}

void isr_handler(uint64_t id, uint64_t stack_addr) {
    stack_t* stack = get_stack(id, stack_addr);

    // We have a special handler for syscalls.
    if (id == SYSCALL) {
        syscall_handler((registers_t*)stack_addr);
        return;
    }

    isr_t handler = interrupt_handlers[id];

    if (handler != 0) {

        handler(stack);
        return;
    }

    PANIC(
        "Received interrupt: %d - %s\n\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %#x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %#x",
        id, exception_messages[id],
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment
    );
}

void irq_handler(uint64_t id, uint64_t stack_addr) {
    if (id >= 40) {
        port_byte_out(PIC2, PIC_EOI);
    }

    port_byte_out(PIC1, PIC_EOI);

    if (interrupt_handlers[id] != 0) {
        isr_t handler = interrupt_handlers[id];
        handler(get_stack(id, stack_addr));
    }
}

void register_interrupt_handler(uint64_t id, isr_t handler) {
    interrupt_handlers[id] = handler;
}

stack_t* get_stack(uint64_t id, uint64_t stack_addr) {
    // See: https://github.com/0xAX/linux-insides/blob/master/interrupts/interrupts-3.md
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
    switch (id) {
    case EXCEPTION_DF:
    case EXCEPTION_TS:
    case EXCEPTION_NP:
    case EXCEPTION_SS:
    case EXCEPTION_GP:
    case EXCEPTION_PF:
    case EXCEPTION_AC:
        // skip error code, so that we always get the same stack_t
        stack_addr += sizeof(uint64_t);
        break;
    }

    return (stack_t*)(stack_addr + sizeof(registers_t));
}

void breakpoint_handler(stack_t* stack) {
    printf(
        "Exception: BREAKPOINT\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %#x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %#x\n",
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment
    );
}

void double_fault_handler(stack_t* stack) {
    printf(
        "Exception: DOUBLE FAULT\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %#x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %#x\n",
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment
    );
}
