/** @file */
#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef intmax_t time_t;
typedef intmax_t suseconds_t;
typedef intmax_t off_t;

#ifndef __pid_t_defined
#if defined(__arm__) && !defined(__aarch64__)
// On 32-bit ARM, `intmax_t` is 64-bit, but the variadic syscall ABI cannot
// express 64-bit arguments. AAPCS wants them on even registers, which the
// assembler stub cannot guarantee. PIDs fit in 32 bits, so use a 32-bit
// `pid_t` on this target.
typedef int32_t pid_t;
#else
typedef intmax_t pid_t;
#endif
#define __pid_t_defined
#endif

#ifndef __mode_t_defined
typedef uint16_t mode_t;
#define __mode_t_defined
#endif

typedef intmax_t ssize_t;

typedef struct opt_uint8
{
  bool has_value;
  uint8_t value;
} opt_uint8_t;

typedef struct opt_uint16
{
  bool has_value;
  uint16_t value;
} opt_uint16_t;

typedef struct opt_uint32
{
  bool has_value;
  uint32_t value;
} opt_uint32_t;

typedef struct opt_uint64
{
  bool has_value;
  uint64_t value;
} opt_uint64_t;

typedef struct opt_bool
{
  bool has_value;
  bool value;
} opt_bool_t;

#endif
