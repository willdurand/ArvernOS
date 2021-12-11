#ifndef ARCH_TASK_H
#define ARCH_TASK_H

#include <stdlib.h>

typedef struct task_info
{
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10; // sl
  uint32_t r11; // fp
  uint32_t cpsr;
  uint32_t sp;
  uint32_t pc;
} __attribute__((packed)) task_info_t;

#endif
