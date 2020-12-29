#include "idt.h"
#include "logging.h"
#include <core/gdt.h>

static idt_gate_t idt[IDT_ENTRIES] = { 0 };

void idt_load()
{
  idt_register_t idt_reg = { .base = (uint64_t)idt, .limit = sizeof(idt) - 1 };
  __asm__("lidt %0" : : "m"(idt_reg));
}

void idt_register_gate(uint16_t n, uint64_t handler, uint8_t type, uint8_t dpl)
{
  idt[n].ptr_low = (uint16_t)handler;
  idt[n].ptr_mid = (uint16_t)(handler >> 16);
  idt[n].ptr_high = (uint32_t)(handler >> 32);
  idt[n].selector = KERNEL_BASE_SELECTOR;
  idt[n].ist = 0;
  idt[n].type = type;
  idt[n].s = 0;
  idt[n].dpl = dpl;
  idt[n].present = 1;

  CORE_DEBUG("registered gate #%03d: handler=%p flags=0x%02x DPL=%d (ring)",
             n,
             handler,
             idt[n].flags,
             dpl);
}

void idt_register_interrupt(uint16_t n, uint64_t handler)
{
  idt_register_gate(n, handler, INTERRUPT_GATE, 0);
}
