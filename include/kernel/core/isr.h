/** @file */
#ifndef CORE_ISR_H
#define CORE_ISR_H

#include <stdint.h>

#define MAX_HANDLERS 256

// Forward declaration.
typedef struct isr_stack isr_stack_t;

/// This type represents an interrupt handler.
typedef void (*isr_handler_t)(isr_stack_t* stack);

/**
 * Initializes the _Interrupt Service Routine_ (ISR).
 */
void isr_init();

/**
 * Enables interrupts.
 */
void isr_enable_interrupts();

/**
 * Disables interrupts.
 */
void isr_disable_interrupts();

/**
 * Registers a handler for a given interrupt. It does not matter whether the
 * interrupt is an exception, a hardware or software interrupt.
 *
 * @param id an interrupt id
 * @param handler the handler to attach to the interrupt
 */
void isr_register_handler(uint32_t id, isr_handler_t handler);

/**
 * Returns the handler corresponding to the interrupt id passed as first
 * argument.
 *
 * @param id an interrupt id
 * @return the handler attached to the interrupt or `0`
 */
isr_handler_t isr_get_handler(uint32_t id);

#endif
