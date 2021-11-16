#include "ubsan.h"
#include <stdio.h>
#include <stdlib.h>
#include <willos/utils.h>

#ifdef __is_libk

#include <logging.h>
#include <panic.h>

#endif

void ubsan_panic_at(ubsan_source_location_t* location, const char* error);

static const char* kinds[] = {
  "load",
  "store",
  "reference binding",
  "member access",
  "member call",
  "constructor call",
  "downcast",
  "downcast",
  "upcast",
  "cast to virtual base",
};

void __ubsan_handle_type_mismatch_v1(ubsan_mismatch_v1_data_t* data,
                                     uintptr_t ptr)
{
  const char* error = "type mismatch (insufficient size)";

  if (!ptr) {
    error = "null pointer access";
  } else if (data->alignment && (ptr & (data->alignment - 1))) {
    error = "unaligned access";
  } else {
#ifdef __is_libk
    DEBUG("ubsan: kind=%s ptr=%p type=%s",
          kinds[data->kind],
          (void*)ptr,
          data->type->name);
    // In non-debug mode, this variable is not used.
    UNUSED(*kinds);
#else
    printf("ubsan: kind=%s ptr=%p type=%s\n",
           kinds[data->kind],
           (void*)ptr,
           data->type->name);
#endif
  }

  ubsan_panic_at(&data->location, error);
}

void __ubsan_handle_add_overflow(ubsan_overflow_data_t* data,
                                 void* lhs,
                                 void* rhs)
{
  UNUSED(*lhs);
  UNUSED(*rhs);

  ubsan_panic_at(&data->location, "add overflow");
}

void __ubsan_handle_sub_overflow(ubsan_overflow_data_t* data,
                                 void* lhs,
                                 void* rhs)
{
  UNUSED(*lhs);
  UNUSED(*rhs);

  ubsan_panic_at(&data->location, "sub overflow");
}

void __ubsan_handle_mul_overflow(ubsan_overflow_data_t* data,
                                 void* lhs,
                                 void* rhs)
{
  UNUSED(*lhs);
  UNUSED(*rhs);

  ubsan_panic_at(&data->location, "mul overflow");
}

void __ubsan_handle_negate_overflow(ubsan_overflow_data_t* data,
                                    void* old_value)
{
  UNUSED(*old_value);

  ubsan_panic_at(&data->location, "negation overflow");
}

void __ubsan_handle_divrem_overflow(ubsan_overflow_data_t* data,
                                    void* lhs,
                                    void* rhs)
{
  UNUSED(*lhs);
  UNUSED(*rhs);

  ubsan_panic_at(&data->location, "negation overflow");
}

void __ubsan_handle_shift_out_of_bounds(ubsan_shift_out_of_bounds_data_t* data,
                                        void* lhs,
                                        void* rhs)
{
  UNUSED(*lhs);
  UNUSED(*rhs);

  ubsan_panic_at(&data->location, "shift out of bounds");
}

void __ubsan_handle_out_of_bounds(ubsan_out_of_bounds_data_t* data, void* index)
{
  UNUSED(*index);

  ubsan_panic_at(&data->location, "shift out of bounds");
}

void __ubsan_handle_load_invalid_value(ubsan_invalid_value_data_t* data,
                                       void* value)
{
  UNUSED(*value);

  ubsan_panic_at(&data->location, "invalid value load");
}

void __ubsan_handle_float_cast_overflow(ubsan_float_cast_overflow_data_t* data,
                                        void* from)
{
  UNUSED(*from);

  ubsan_panic_at(&data->location, "float cast overflow");
}

void __ubsan_handle_pointer_overflow(ubsan_pointer_overflow_data_t* data,
                                     void* lhs,
                                     void* rhs)
{
  UNUSED(*lhs);
  UNUSED(*rhs);

  ubsan_panic_at(&data->location, "pointer overflow");
}

void __ubsan_handle_vla_bound_not_positive(ubsan_vla_bound_data_t* data,
                                           void* bound)
{
  UNUSED(*bound);

  ubsan_panic_at(&data->location, "vla bound not positive");
}

void ubsan_panic_at(ubsan_source_location_t* location, const char* error)
{
#ifdef __is_libk
  if (location) {
    PANIC("ubsan: file=%s line=%d column=%d error=%s",
          location->file,
          location->line,
          location->column,
          error);
  } else {
    PANIC("ubsan: file=(unknown) line=(unknown) column=(unknown) error=%s",
          error);
  }
#else
  if (location) {
    printf("ubsan: file=%s line=%d column=%d error=%s\n",
           location->file,
           location->line,
           location->column,
           error);
  } else {
    printf("ubsan: file=(unknown) line=(unknown) column=(unknown) error=%s\n",
           error);
  }
  abort();
#endif
}
