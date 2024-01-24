#include "ubsan.h"
#include <arvern/utils.h>
#include <stdio.h>
#include <stdlib.h>

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

void __ubsan_handle_type_mismatch(ubsan_mismatch_data_t* data, uintptr_t ptr)
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
    UNUSED(kinds);
#else
    printf("ubsan: kind=%s ptr=%p type=%s\n",
           kinds[data->kind],
           (void*)ptr,
           data->type->name);
#endif
  }

  ubsan_panic_at(&data->location, error);
}

void __ubsan_handle_type_mismatch_v1(ubsan_mismatch_v1_data_t* data,
                                     uintptr_t ptr)
{
  ubsan_mismatch_data_t old_data = {
    .location = data->location,
    .type = data->type,
    .alignment = 1UL << data->log_alignment,
    .kind = data->kind,
  };

  __ubsan_handle_type_mismatch(&old_data, ptr);
}

void __ubsan_handle_add_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "add overflow");
}

void __ubsan_handle_sub_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "sub overflow");
}

void __ubsan_handle_mul_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "mul overflow");
}

void __ubsan_handle_negate_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "negation overflow");
}

void __ubsan_handle_divrem_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "negation overflow");
}

void __ubsan_handle_shift_out_of_bounds(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "shift out of bounds");
}

void __ubsan_handle_out_of_bounds(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "shift out of bounds");
}

void __ubsan_handle_load_invalid_value(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "invalid value load");
}

void __ubsan_handle_float_cast_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "float cast overflow");
}

void __ubsan_handle_pointer_overflow(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "pointer overflow");
}

void __ubsan_handle_vla_bound_not_positive(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "vla bound not positive");
}

void __ubsan_handle_invalid_builtin(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "invalid built-in");
}

void __ubsan_handle_function_type_mismatch(ubsan_source_location_t* location)
{
  ubsan_panic_at(location, "function type mismatch");
}

void ubsan_panic_at(ubsan_source_location_t* location, const char* error)
{
#ifdef __is_libk
  if (location) {
    PANIC("ubsan: file=%s line=%ld column=%ld error=%s",
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
    printf("ubsan: file=%s line=%ld column=%ld error=%s\n",
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