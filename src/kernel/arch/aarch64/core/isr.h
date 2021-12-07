/** @file */
#ifndef CORE_ISR_H
#define CORE_ISR_H

#include <stdint.h>

/// This type represents an interrupt handler.
typedef void (*isr_handler_t)();

/**
 * Initializes the _Interrupt Service Routine_ (ISR).
 */
void isr_init();

/**
 * Enables hardware interrupts.
 */
void isr_enable_interrupts();

/**
 * Disables hardware interrupts.
 */
void isr_disable_interrupts();

void isr_irq_handler();

void isr_exception_handler(uint8_t type);

void isr_register_handler(uint32_t id, isr_handler_t handler);

#endif
