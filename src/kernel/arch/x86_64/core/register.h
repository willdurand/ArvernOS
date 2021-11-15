/** @file */
#ifndef CORE_REGISTER_H
#define CORE_REGISTER_H

#include <stdint.h>

/**
 * Extended Feature Enables
 * @see https://wiki.osdev.org/CPU_Registers_x86-64#IA32_EFER
 */
#define IA32_EFER 0xC0000080
/// System Call Target Address
#define IA32_STAR 0xC0000081
/// IA-32e Mode System Call Target Address
#define IA32_LSTAR 0xC0000082
/// System Call Flag Mask
#define IA32_SFMASK 0xC0000084

/**
 * Returns the value of the CR0 register.
 *
 * @return the address contained in the CR0 register
 */
uint64_t read_cr0();

/**
 * Returns the value of the CR2 register.
 *
 * @return the address contained in the CR2 register
 */
uint64_t read_cr2();

/**
 * Returns the value of the CR3 register.
 *
 * @return the address contained in the CR3 register
 */
uint64_t read_cr3();

/**
 * Updates the value of the CR0 register.
 *
 * @param value the new value for the CR0 register
 */
void write_cr0(uint64_t value);

/**
 * Updates the value of the CR3 register.
 *
 * @param value the new value for the CR3 register
 */
void write_cr3(uint64_t value);

/**
 * Read a Model Specific Register.
 * @see https://wiki.osdev.org/Model_Specific_Registers
 *
 * @return the value for the specified register
 */
uint64_t read_msr(uint64_t msr);

/**
 * Updates the value of the given Model Specific Register.
 * @see https://wiki.osdev.org/Model_Specific_Registers
 *
 * @param msr the model specific register number
 * @param value the new value
 */
void write_msr(uint64_t msr, uint64_t value);

/**
 * Returns the value of the RSP register (stack pointer).
 *
 * @return the address contained in the RSP register
 */
uint64_t read_rsp();

#endif
