#ifndef ARCH_TASK_H
#define ARCH_TASK_H

#include <stdlib.h>

typedef struct task_info
{
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uintptr_t fp;
  uintptr_t sp;
  uintptr_t pc;
} __attribute__((packed)) task_info_t;

#endif
