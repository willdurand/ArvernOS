#ifndef ARCH_TASK_H
#define ARCH_TASK_H

#include <stdlib.h>

typedef struct task_info
{
  uint64_t rsp;
} __attribute__((packed)) task_info_t;

#endif
