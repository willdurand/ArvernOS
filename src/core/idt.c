#include "idt.h"
#include <core/gdt.h>

static idt_gate_t idt[IDT_ENTRIES] = { 0 };

void idt_init()
{
  idt_register_t idt_reg = { .base = (uint64_t)idt, .limit = sizeof(idt) - 1 };
  __asm__("lidt %0" : : "m"(idt_reg));
}

void idt_register_gate(uint16_t n, uint64_t handler, uint8_t type)
{
  idt[n].selector = KERNEL_BASE_SELECTOR;
  idt[n].ptr_low = (uint16_t)handler;
  idt[n].ptr_mid = (uint16_t)(handler >> 16);
  idt[n].ptr_high = (uint32_t)(handler >> 32);
  idt[n].reserved = 0;

  idt[n].options.index = 0; // do not switch stack
  idt[n].options.type = type;
  idt[n].options.ones = 0x07;
  idt[n].options.zero = 0;
  idt[n].options.DPL = 0; // privilege to call the handler ("ring")
  idt[n].options.present = 1;
}

void idt_register_interrupt(uint16_t n, uint64_t handler)
{
  idt_register_gate(n, handler, INTERRUPT_GATE);
}
