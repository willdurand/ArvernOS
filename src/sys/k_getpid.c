#include "k_syscall.h"
#include <proc/process.h>

pid_t k_getpid()
{
  process_t* current_process = process_get_current();

  return current_process->pid;
}
