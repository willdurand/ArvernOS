#include "isr.h"
#include <core/debug.h>
#include <core/idt.h>
#include <core/port.h>
#include <core/register.h>
#include <core/syscall.h>
#include <kernel/panic.h>
#include <stdio.h>
#include <stdlib.h>

stack_t* get_stack(uint64_t id, uint64_t stack);

void breakpoint_handler(stack_t* stack);
void double_fault_handler(stack_t* stack);
void page_fault_handler(stack_t* stack);

const char* exception_messages[] = { "Division By Zero",
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
                                     "Reserved" };

isr_t interrupt_handlers[256];

void isr_init()
{
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

  idt_register_interrupt(0, (uint64_t)isr0);
  idt_register_interrupt(1, (uint64_t)isr1);
  idt_register_interrupt(2, (uint64_t)isr2);
  idt_register_interrupt(3, (uint64_t)isr3);
  idt_register_interrupt(4, (uint64_t)isr4);
  idt_register_interrupt(5, (uint64_t)isr5);
  idt_register_interrupt(6, (uint64_t)isr6);
  idt_register_interrupt(7, (uint64_t)isr7);
  idt_register_interrupt(8, (uint64_t)isr8);
  idt_register_interrupt(9, (uint64_t)isr9);
  idt_register_interrupt(10, (uint64_t)isr10);
  idt_register_interrupt(11, (uint64_t)isr11);
  idt_register_interrupt(12, (uint64_t)isr12);
  idt_register_interrupt(13, (uint64_t)isr13);
  idt_register_interrupt(14, (uint64_t)isr14);
  idt_register_interrupt(15, (uint64_t)isr15);
  idt_register_interrupt(16, (uint64_t)isr16);
  idt_register_interrupt(17, (uint64_t)isr17);
  idt_register_interrupt(18, (uint64_t)isr18);
  idt_register_interrupt(19, (uint64_t)isr19);
  idt_register_interrupt(20, (uint64_t)isr20);
  idt_register_interrupt(21, (uint64_t)isr21);
  idt_register_interrupt(22, (uint64_t)isr22);
  idt_register_interrupt(23, (uint64_t)isr23);
  idt_register_interrupt(24, (uint64_t)isr24);
  idt_register_interrupt(25, (uint64_t)isr25);
  idt_register_interrupt(26, (uint64_t)isr26);
  idt_register_interrupt(27, (uint64_t)isr27);
  idt_register_interrupt(28, (uint64_t)isr28);
  idt_register_interrupt(29, (uint64_t)isr29);
  idt_register_interrupt(30, (uint64_t)isr30);
  idt_register_interrupt(31, (uint64_t)isr31);

  idt_register_interrupt(IRQ0, (uint64_t)irq0);
  idt_register_interrupt(IRQ1, (uint64_t)irq1);
  idt_register_interrupt(IRQ2, (uint64_t)irq2);
  idt_register_interrupt(IRQ3, (uint64_t)irq3);
  idt_register_interrupt(IRQ4, (uint64_t)irq4);

  // syscalls
  idt_register_interrupt(SYSCALL, (uint64_t)isr0x80);

  // handlers for isr exceptions
  isr_register_handler(EXCEPTION_BP, breakpoint_handler);
  isr_register_handler(EXCEPTION_DF, double_fault_handler);
  isr_register_handler(EXCEPTION_PF, page_fault_handler);

  idt_init();
}

void irq_init()
{
  __asm__("sti");
}

void irq_disable()
{
  __asm__("cli");
}

void isr_handler(uint64_t id, uint64_t stack_addr)
{
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

  PANIC("Received interrupt: %d - %s\n\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %x",
        id,
        exception_messages[id],
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment);
}

void irq_handler(uint64_t id, uint64_t stack_addr)
{
  if (id >= 40) {
    port_byte_out(PIC2, PIC_EOI);
  }

  port_byte_out(PIC1, PIC_EOI);

  if (interrupt_handlers[id] != 0) {
    isr_t handler = interrupt_handlers[id];
    handler(get_stack(id, stack_addr));
  }
}

void isr_register_handler(uint64_t id, isr_t handler)
{
  interrupt_handlers[id] = handler;
}

stack_t* get_stack(uint64_t id, uint64_t stack_addr)
{
  // See:
  // https://github.com/0xAX/linux-insides/blob/master/interrupts/interrupts-3.md
  // See:
  // https://github.com/littleosbook/littleosbook/blob/e90faeb24c5c9fed8cde9a35974893706e81cbbf/interrupts.md
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

void breakpoint_handler(stack_t* stack)
{
  printf("Exception: BREAKPOINT\n"
         "  instruction_pointer = %p\n"
         "  code_segment        = %x\n"
         "  cpu_flags           = %#x\n"
         "  stack_pointer       = %p\n"
         "  stack_segment       = %x\n",
         stack->instruction_pointer,
         stack->code_segment,
         stack->cpu_flags,
         stack->stack_pointer,
         stack->stack_segment);
}

void double_fault_handler(stack_t* stack)
{
  PANIC("Exception: DOUBLE FAULT\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %x\n",
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment);
}

void page_fault_handler(stack_t* stack)
{
  // The `get_stack()` function removes the error code so that we can have
  // generic handlers (and not all interruptions have an error code anyway).
  // This handler needs the `error_code`, so let's retrieve it.
  uint64_t error_code = ((uint64_t*)stack)[-1];

  PANIC("Exception: PAGE FAULT\n"
        "  accessed address    = %p\n"
        "  error code          = %#x\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %x\n",
        read_cr2(),
        error_code,
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment);
}
