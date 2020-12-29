/**
 * @file
 * @see https://wiki.osdev.org/Interrupt_Descriptor_Table
 * @see https://os.phil-opp.com/handling-exceptions/
 */
#ifndef CORE_IDT_H
#define CORE_IDT_H

#include <stdint.h>

#define INTERRUPT_GATE 0xE
#define TRAP_GATE      0xF

/**
 * The number of entries in the _Interrupt Descriptor Table_.
 *
 * There are 256 interrupts (0..255), so IDT should have 256 entries, each
 * entry corresponding to a specific interrupt.
 */
#define IDT_ENTRIES 256

/**
 * This structure represents an IDT gate. A gate is the name of an entry in the
 * IDT, which can contain Interrupt Gates, Task Gates and Trap Gates.
 */
typedef struct idt_gate
{
  uint16_t ptr_low;
  uint16_t selector;
  uint8_t ist;
  union
  {
    uint8_t flags;
    struct
    {
      uint8_t type : 4;
      uint8_t s : 1;
      uint8_t dpl : 2;
      uint8_t present : 1;
    };
  };
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
 * Loads the _Interrupt Descriptor Table_. The table should be updated with
 * `idt_register_gate()` and/or `idt_register_interrupt()` before calling this
 * function.
 */
void idt_load();

/**
 * Registers a gate in the IDT.
 *
 * @param n a gate number
 * @param handler the handler to register for the given gate
 * @param type the gate type
 * @param dpl the DPL value ("ring")
 */
void idt_register_gate(uint16_t n, uint64_t handler, uint8_t type, uint8_t dpl);

/**
 * Registers a an interrupt gate in the IDT.
 *
 * @param n a gate number
 * @param handler the handler to register for the given gate
 */

void idt_register_interrupt(uint16_t n, uint64_t handler);

#endif
