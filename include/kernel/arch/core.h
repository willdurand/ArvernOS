/** @file */
#ifndef ARCH_CORE_H
#define ARCH_CORE_H

/**
 * Initializes the Interrupt Service Routines (ISR).
 */
void arch_isr_init();

/**
 * Enables interrupts.
 */
void arch_isr_enable_interrupts();

/**
 * Disables interrupts.
 */
void arch_isr_disable_interrupts();

#endif
