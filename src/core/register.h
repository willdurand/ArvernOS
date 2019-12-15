/** @file */
#ifndef CORE_REGISTER_H
#define CORE_REGISTER_H

#include <stdint.h>

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
 * Updates the value of the CR3 register.
 *
 * @param value the new value for the CR3 register
 */
void write_cr3(uint64_t value);

#endif
