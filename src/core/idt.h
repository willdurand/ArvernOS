/** @file */
#ifndef CORE_IDT_H
#define CORE_IDT_H

#include <stdint.h>

/// The number of entries in the _Interrupt Descriptor Table_.
#define IDT_ENTRIES 256

/**
 * This structure represents the _Interrupt Descriptor Table_ options.
 */
typedef struct idt_opts {
    uint8_t stack_OK  : 3;
    uint8_t ZEROS     : 5;
    uint8_t gate_type : 1;
    uint8_t ONES      : 3;
    uint8_t ZERO      : 1;
    uint8_t DPL       : 2;
    uint8_t present   : 1;
} __attribute__((packed)) idt_opts_t;

typedef struct idt_gate {
    uint16_t ptr_low;
    uint16_t selector;
    idt_opts_t opts;
    uint16_t ptr_mid;
    uint32_t ptr_high;
    uint8_t  _1_reserved : 8;
    uint8_t  _type       : 5;
    uint32_t _2_reserved : 19;
} __attribute__((packed)) idt_gate_t;

typedef struct idt_register {
    uint16_t length;
    uint64_t base;
} __attribute__((packed)) idt_register_t;

/**
 * Initializes the _Interrupt Descriptor Table_.
 */
void idt_init();

/**
 * Registers a _handler_ for a _gate_.
 *
 * @param n a gate number
 * @param handler the handle to register for the given gate
 */
void register_idt_gate(uint16_t n, uint64_t handler);

#endif
