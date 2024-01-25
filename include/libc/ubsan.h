/**
 * @file
 * @see https://wiki.osdev.org/Undefined_Behavior_Sanitization
 */
#ifndef UBSAN_H
#define UBSAN_H

#include <stdint.h>

typedef struct ubsan_type
{
  uint16_t kind;
  uint16_t info;
  char name[];
} ubsan_type_t;

typedef struct ubsan_source_location
{
  const char* file;
  uint32_t line;
  uint32_t column;
} ubsan_source_location_t;

typedef struct ubsan_mismatch_data
{
  ubsan_source_location_t location;
  ubsan_type_t* type;
  uintptr_t alignment;
  uint8_t kind;
} ubsan_mismatch_data_t;

typedef struct ubsan_mismatch_v1_data
{
  ubsan_source_location_t location;
  ubsan_type_t* type;
  uint8_t log_alignment;
  uint8_t kind;
} ubsan_mismatch_v1_data_t;

void __ubsan_handle_type_mismatch(ubsan_mismatch_data_t* data, uintptr_t ptr);

// This function is suffixed with _v1 because Clang and GCC 8 slightly changed
// ABI for 'type mismatch' errors, so compilers now use this function.
//
// See:
// https://patches.linaro.org/project/lkml/patch/20180208154636.21320-1-mark.rutland@arm.com/
void __ubsan_handle_type_mismatch_v1(ubsan_mismatch_v1_data_t* data,
                                     uintptr_t ptr);

void __ubsan_handle_add_overflow(ubsan_source_location_t* location);

void __ubsan_handle_sub_overflow(ubsan_source_location_t* location);

void __ubsan_handle_mul_overflow(ubsan_source_location_t* location);

void __ubsan_handle_negate_overflow(ubsan_source_location_t* location);

void __ubsan_handle_divrem_overflow(ubsan_source_location_t* location);

void __ubsan_handle_shift_out_of_bounds(ubsan_source_location_t* location);

void __ubsan_handle_out_of_bounds(ubsan_source_location_t* location);

void __ubsan_handle_load_invalid_value(ubsan_source_location_t* location);

void __ubsan_handle_float_cast_overflow(ubsan_source_location_t* location);

void __ubsan_handle_pointer_overflow(ubsan_source_location_t* location);

void __ubsan_handle_vla_bound_not_positive(ubsan_source_location_t* location);

void __ubsan_handle_invalid_builtin(ubsan_source_location_t* location);

void __ubsan_handle_function_type_mismatch(ubsan_source_location_t* location);

#endif
