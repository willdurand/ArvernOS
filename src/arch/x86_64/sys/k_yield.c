#include "k_syscall.h"
#include <proc/process.h>

int k_yield()
{
  process_yield();

  return 0;
}
