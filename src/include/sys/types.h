#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef int64_t time_t;
typedef int64_t suseconds_t;

#ifndef __is_libc
typedef struct opt_uint8 {
    bool is_valid;
    uint8_t value;
} opt_uint8_t;

typedef struct opt_uint16 {
    bool is_valid;
    uint16_t value;
} opt_uint16_t;

typedef struct opt_uint32 {
    bool is_valid;
    uint32_t value;
} opt_uint32_t;

typedef struct opt_uint64 {
    bool is_valid;
    uint64_t value;
} opt_uint64_t;

#endif

#endif
