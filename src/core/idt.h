/**
 * @file
 * @see https://wiki.osdev.org/Interrupt_Descriptor_Table
 * @see https://os.phil-opp.com/handling-exceptions/
 */
#ifndef CORE_IDT_H
#define CORE_IDT_H

#include <stdint.h>

#define INTERRUPT_GATE 0x01
#define TRAP_GATE      0x02

/**
 * The number of entries in the _Interrupt Descriptor Table_.
 *
 * There are 256 interrupts (0..255), so IDT should have 256 entries, each
 * entry corresponding to a specific interrupt.
 */
#define IDT_ENTRIES 256

typedef struct idt_gate_options
{
  uint8_t index : 3;
  uint8_t reserved : 5;
  uint8_t type : 1;
  uint8_t ones : 3;
  uint8_t zero : 1;
  uint8_t DPL : 2;
  uint8_t present : 1;
} __attribute__((packed)) idt_gate_options_t;

/**
 * This structure represents an IDT gate.
 */
typedef struct idt_gate
{
  uint16_t ptr_low;
  uint16_t selector;
  idt_gate_options_t options;
  uint16_t ptr_mid;
  uint32_t ptr_high;
  uint32_t reserved;
} __attribute__((packed)) idt_gate_t;

/**
 * This structure represents the IDT register.
 */
typedef struct idt_register
{
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) idt_register_t;

/**
 * Initializes the _Interrupt Descriptor Table_.
 */
void idt_init();

/**
 * Registers a gate in the IDT.
 *
 * @param n a gate number
 * @param handler the handler to register for the given gate
 * @param type the gate type
 */
void idt_register_gate(uint16_t n, uint64_t handler, uint8_t type);

/**
 * Registers a an interrupt gate in the IDT.
 *
 * @param n a gate number
 * @param handler the handler to register for the given gate
 */

void idt_register_interrupt(uint16_t n, uint64_t handler);

#endif
