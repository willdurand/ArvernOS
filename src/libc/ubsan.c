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

#ifdef __is_aarch32

// This is needed to fix the following issue on AArch32:
//
// ```
// ld.lld: error: undefined symbol: abort
// >>> referenced by unwind-arm.c
// >>>               unwind-arm.o:(unwind_phase2)
// ```
//
// While this error is about `abort()`, the underlying issue is that we build
// an object file that references the `__aeabi_unwind_cpp_pr0` symbol for this
// module. This can be observed with:
//
// ```
// $ llvm-objdump -x build/aarch32/raspi2/libk-objects/src/libc/ubsan.o
//
// [...]
//
// RELOCATION RECORDS FOR [.ARM.exidx.text.__ubsan_handle_type_mismatch_v1]:
// OFFSET   TYPE                     VALUE
// 00000000 R_ARM_NONE               __aeabi_unwind_cpp_pr0
//
// [...]
// ```
//
// Now, I am not sure why `-fno-exceptions` doesn't prevent this to be honest.
//
// Anyway, the reason why the stacktrace above mentions `abort()` is because we
// also link to libgcc on AArch32 (because we have to target the ARM Embedded
// ABI), and libgcc is bundled with libunwind, hence `unwind-arm.c`. And this
// calls `abort()` at some point [1] .
//
// In libk, it is expected that we do not have an `abort()` function but it
// looks like the libgcc we're using should be patched or something if we
// wanted to fully get rid of the exception/unwind (generated) code [2] [3].
//
// Sigh.
//
// The approach chosen here is to avoid linking this `__aeabi_unwind_cpp_pr0`
// symbol against libgcc by defining an empty function in the same compilation
// module instead.
//
// [1]:
// https://github.com/gcc-mirror/gcc/blob/9693459e030977d6e906ea7eb587ed09ee4fddbd/libgcc/unwind-arm-common.inc#L436
// [2]: https://gcc.gnu.org/legacy-ml/gcc-help/2012-03/msg00372.html
// [3]:
// https://github.com/Rockbox/www/blob/9ecaa836d52dbf15113c93a7c447d11323e75aaf/gcc/rockbox-multilibs-noexceptions-arm-elf-eabi-gcc-4.4.2_1.diff#L2
void __aeabi_unwind_cpp_pr0() {}

#endif
