#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef int64_t time_t;
typedef int64_t suseconds_t;
typedef uint64_t size_t;
typedef int64_t ssize_t;

#ifndef __is_libc
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

#endif

#endif
