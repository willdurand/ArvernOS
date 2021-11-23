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

typedef struct ubsan_mismatch_v1_data
{
  ubsan_source_location_t location;
  ubsan_type_t* type;
  uint8_t alignment;
  uint8_t kind;
} ubsan_mismatch_v1_data_t;

typedef struct ubsan_overflow_data
{
  ubsan_source_location_t location;
  ubsan_type_t* type;
} ubsan_overflow_data_t;

typedef struct ubsan_shift_out_of_bounds_data
{
  ubsan_source_location_t location;
  ubsan_type_t* lhs_type;
  ubsan_type_t* rhs_type;
} ubsan_shift_out_of_bounds_data_t;

typedef struct ubsan_out_of_bounds_data
{
  ubsan_source_location_t location;
  ubsan_type_t* array_type;
  ubsan_type_t* index_type;
} ubsan_out_of_bounds_data_t;

typedef struct ubsan_invalid_value_data
{
  ubsan_source_location_t location;
  ubsan_type_t* type;
} ubsan_invalid_value_data_t;

typedef struct ubsan_float_cast_overflow_data
{
  ubsan_source_location_t location;
  ubsan_type_t* from_type;
  ubsan_type_t* to_type;
} ubsan_float_cast_overflow_data_t;

typedef struct ubsan_pointer_overflow_data
{
  ubsan_source_location_t location;
} ubsan_pointer_overflow_data_t;

typedef struct ubsan_vla_bound_data
{
  ubsan_source_location_t location;
  ubsan_type_t* type;
} ubsan_vla_bound_data_t;

typedef struct ubsan_invalid_builtin_data
{
  ubsan_source_location_t location;
  uint8_t kind;
} ubsan_invalid_builtin_data_t;

// This function is suffixed with _v1 because Clang and GCC 8 slightly changed
// ABI for 'type mismatch' errors, so compilers now use this function.
void __ubsan_handle_type_mismatch_v1(ubsan_mismatch_v1_data_t* data,
                                     uintptr_t ptr);

void __ubsan_handle_add_overflow(ubsan_overflow_data_t* data,
                                 void* lhs,
                                 void* rhs);

void __ubsan_handle_sub_overflow(ubsan_overflow_data_t* data,
                                 void* lhs,
                                 void* rhs);

void __ubsan_handle_mul_overflow(ubsan_overflow_data_t* data,
                                 void* lhs,
                                 void* rhs);

void __ubsan_handle_negate_overflow(ubsan_overflow_data_t* data,
                                    void* old_value);

void __ubsan_handle_divrem_overflow(ubsan_overflow_data_t* data,
                                    void* lhs,
                                    void* rhs);

void __ubsan_handle_shift_out_of_bounds(ubsan_shift_out_of_bounds_data_t* data,
                                        void* lhs,
                                        void* rhs);

void __ubsan_handle_out_of_bounds(ubsan_out_of_bounds_data_t* data,
                                  void* index);

void __ubsan_handle_load_invalid_value(ubsan_invalid_value_data_t* data,
                                       void* value);

void __ubsan_handle_float_cast_overflow(ubsan_float_cast_overflow_data_t* data,
                                        void* from);

void __ubsan_handle_pointer_overflow(ubsan_pointer_overflow_data_t* data,
                                     void* lhs,
                                     void* rhs);

void __ubsan_handle_vla_bound_not_positive(ubsan_vla_bound_data_t* data,
                                           void* bound);

void __ubsan_handle_invalid_builtin(ubsan_invalid_builtin_data_t* data);

#endif
