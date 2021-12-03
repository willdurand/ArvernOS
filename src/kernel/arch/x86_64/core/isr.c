#include "isr.h"
#include <core/idt.h>
#include <core/logging.h>
#include <core/port.h>
#include <core/register.h>
#include <inttypes.h>
#include <panic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void breakpoint_handler(isr_stack_t* stack);
void page_fault_handler(isr_stack_t* stack);

static const char* exception_messages[] = { "Division By Zero",
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

static isr_handler_t handlers[256] = { 0 };

void isr_init()
{
  // start initialization
  port_byte_out(PIC1, 0x11);
  port_byte_out(PIC2, 0x11);

  // set IRQ base numbers for each PIC
  port_byte_out(PIC1_DATA, IRQ_BASE);
  port_byte_out(PIC2_DATA, IRQ_BASE + 8);

  // use IRQ number 2 to relay IRQs from the secondary PIC
  port_byte_out(PIC1_DATA, 0x04);
  port_byte_out(PIC2_DATA, 0x02);

  // finish initialization
  port_byte_out(PIC1_DATA, 0x01);
  port_byte_out(PIC2_DATA, 0x01);

  // mask all IRQs
  port_byte_out(PIC1_DATA, 0x00);
  port_byte_out(PIC2_DATA, 0x00);

  // Exceptions
  idt_register_interrupt(0, (uint64_t)exc0);
  idt_register_interrupt(1, (uint64_t)exc1);
  idt_register_interrupt(2, (uint64_t)exc2);
  idt_register_interrupt(3, (uint64_t)exc3);
  idt_register_interrupt(4, (uint64_t)exc4);
  idt_register_interrupt(5, (uint64_t)exc5);
  idt_register_interrupt(6, (uint64_t)exc6);
  idt_register_interrupt(7, (uint64_t)exc7);
  idt_register_interrupt(8, (uint64_t)exc8);
  idt_register_interrupt(9, (uint64_t)exc9);
  idt_register_interrupt(10, (uint64_t)exc10);
  idt_register_interrupt(11, (uint64_t)exc11);
  idt_register_interrupt(12, (uint64_t)exc12);
  idt_register_interrupt(13, (uint64_t)exc13);
  idt_register_interrupt(14, (uint64_t)exc14);
  idt_register_interrupt(15, (uint64_t)exc15);
  idt_register_interrupt(16, (uint64_t)exc16);
  idt_register_interrupt(17, (uint64_t)exc17);
  idt_register_interrupt(18, (uint64_t)exc18);
  idt_register_interrupt(19, (uint64_t)exc19);
  idt_register_interrupt(20, (uint64_t)exc20);
  idt_register_interrupt(21, (uint64_t)exc21);
  idt_register_interrupt(22, (uint64_t)exc22);
  idt_register_interrupt(23, (uint64_t)exc23);
  idt_register_interrupt(24, (uint64_t)exc24);
  idt_register_interrupt(25, (uint64_t)exc25);
  idt_register_interrupt(26, (uint64_t)exc26);
  idt_register_interrupt(27, (uint64_t)exc27);
  idt_register_interrupt(28, (uint64_t)exc28);
  idt_register_interrupt(29, (uint64_t)exc29);
  idt_register_interrupt(30, (uint64_t)exc30);
  idt_register_interrupt(31, (uint64_t)exc31);

  // Hardware interrupts
  idt_register_interrupt(IRQ0, (uint64_t)irq0);
  idt_register_interrupt(IRQ1, (uint64_t)irq1);
  idt_register_interrupt(IRQ2, (uint64_t)irq2);
  idt_register_interrupt(IRQ3, (uint64_t)irq3);
  idt_register_interrupt(IRQ4, (uint64_t)irq4);
  idt_register_interrupt(IRQ5, (uint64_t)irq5);
  idt_register_interrupt(IRQ6, (uint64_t)irq6);
  idt_register_interrupt(IRQ7, (uint64_t)irq7);
  idt_register_interrupt(IRQ8, (uint64_t)irq8);
  idt_register_interrupt(IRQ9, (uint64_t)irq9);
  idt_register_interrupt(IRQ10, (uint64_t)irq10);
  idt_register_interrupt(IRQ11, (uint64_t)irq11);
  idt_register_interrupt(IRQ12, (uint64_t)irq12);

  // Specific handlers for exceptions.
  isr_register_handler(EXCEPTION_BP, breakpoint_handler);
  isr_register_handler(EXCEPTION_PF, page_fault_handler);

  idt_load();

  isr_enable_interrupts();
}

void isr_enable_interrupts()
{
  __asm__("sti");
}

void isr_disable_interrupts()
{
  __asm__("cli");
}

void isr_int_handler(isr_stack_t stack)
{
  isr_handler_t handler = handlers[stack.id];

  if (handler != 0) {
    handler(&stack);
    return;
  }

  PANIC("received interrupt (see below)\n\n"
        "  %d - %s\n\n"
        "  error_code          = %#x\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %#x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %#x\n"
        "\n"
        "  rax = 0x%08x    rbx = 0x%08x    rcx = 0x%08x\n"
        "  rdx = 0x%08x    rsi = 0x%08x    rdi = 0x%08x\n"
        "  rbp = 0x%08x    r8  = 0x%08x    r9  = 0x%08x\n"
        "  r10 = 0x%08x    r11 = 0x%08x    r12 = 0x%08x\n"
        "  r13 = 0x%08x    r14 = 0x%08x    r15 = 0x%08x",
        stack.id,
        exception_messages[stack.id],
        stack.error_code,
        stack.instruction_pointer,
        stack.code_segment,
        stack.cpu_flags,
        stack.stack_pointer,
        stack.stack_segment,
        stack.rax,
        stack.rbx,
        stack.rcx,
        stack.rdx,
        stack.rsi,
        stack.rdi,
        stack.rbp,
        stack.r8,
        stack.r9,
        stack.r10,
        stack.r11,
        stack.r12,
        stack.r13,
        stack.r14,
        stack.r15);
}

void isr_irq_handler(isr_stack_t stack)
{
  if (handlers[stack.id] != 0) {
    isr_handler_t handler = handlers[stack.id];
    handler(&stack);
  }

  if (stack.id >= 40) {
    port_byte_out(PIC2, PIC_EOI);
  }

  port_byte_out(PIC1, PIC_EOI);
}

void isr_register_handler(uint64_t id, isr_handler_t handler)
{
  handlers[id] = handler;
}

void breakpoint_handler(isr_stack_t* stack)
{
  printf("Exception: BREAKPOINT\n"
         "  instruction_pointer = %p\n"
         "  code_segment        = %" PRIx64 "\n"
         "  cpu_flags           = %#" PRIx64 "\n"
         "  stack_pointer       = %p\n"
         "  stack_segment       = %" PRIx64 "\n",
         stack->instruction_pointer,
         stack->code_segment,
         stack->cpu_flags,
         stack->stack_pointer,
         stack->stack_segment);
}

void page_fault_handler(isr_stack_t* stack)
{
  uint64_t error_code = stack->error_code;
  uint8_t is_present = (error_code >> 0) & 1;
  uint8_t is_write = (error_code >> 1) & 1;
  uint8_t is_user = (error_code >> 2) & 1;
  uint8_t is_reserved_write = (error_code >> 3) & 1;
  uint8_t is_instruction_fetch = (error_code >> 4) & 1;

  PANIC("Exception: PAGE FAULT\n"
        "  accessed address    = %p\n"
        "  error_code          = %#x\n"
        "  error details:\n"
        "    present           = %c\n"
        "    write             = %c\n"
        "    user              = %c\n"
        "    reserved write    = %c\n"
        "    instruction fetch = %c\n"
        "  instruction_pointer = %p\n"
        "  code_segment        = %#x\n"
        "  cpu_flags           = %#x\n"
        "  stack_pointer       = %p\n"
        "  stack_segment       = %#x\n"
        "\n"
        "  rax = 0x%08x    rbx = 0x%08x    rcx = 0x%08x\n"
        "  rdx = 0x%08x    rsi = 0x%08x    rdi = 0x%08x\n"
        "  rbp = 0x%08x    r8  = 0x%08x    r9  = 0x%08x\n"
        "  r10 = 0x%08x    r11 = 0x%08x    r12 = 0x%08x\n"
        "  r13 = 0x%08x    r14 = 0x%08x    r15 = 0x%08x",
        read_cr2(),
        error_code,
        is_present != 0 ? 'Y' : 'N',
        is_write != 0 ? 'Y' : 'N',
        is_user != 0 ? 'Y' : 'N',
        is_reserved_write != 0 ? 'Y' : 'N',
        is_instruction_fetch != 0 ? 'Y' : 'N',
        stack->instruction_pointer,
        stack->code_segment,
        stack->cpu_flags,
        stack->stack_pointer,
        stack->stack_segment,
        stack->rax,
        stack->rbx,
        stack->rcx,
        stack->rdx,
        stack->rsi,
        stack->rdi,
        stack->rbp,
        stack->r8,
        stack->r9,
        stack->r10,
        stack->r11,
        stack->r12,
        stack->r13,
        stack->r14,
        stack->r15);
}
