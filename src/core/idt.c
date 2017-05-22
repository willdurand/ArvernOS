#include "idt.h"

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(uint16_t n, uint64_t handler)
{
    idt[n].selector = 0x08;
    idt[n].ptr_low  = (uint16_t) handler;
    idt[n].ptr_mid  = (uint16_t) (handler >> 16);
    idt[n].ptr_high = (uint32_t) (handler >> 32);

    idt[n].opts.stack_OK  = 0; // do not switch stack
    idt[n].opts.present   = 1; // are we valid
    idt[n].opts.DPL       = 3; // priv to call int handler
    idt[n].opts.gate_type = 0x01; // 1 = interrupt, 2 = trap
    idt[n].opts.ONES      = 0x07;
    idt[n].opts.ZERO      = 0;
    idt[n].opts.ZEROS     = 0;

    idt[n]._1_reserved = 0;
    idt[n]._2_reserved = 0;
    idt[n]._type       = 0;
}

void set_idt()
{
    idt_reg.base = (uint64_t) &idt;
    idt_reg.length = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__("lidt %0" : : "m"(idt_reg));
}
