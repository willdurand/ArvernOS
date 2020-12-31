#include "k_syscall.h"
#include <proc/process.h>

extern process_t* current_process;

pid_t k_getpid()
{
  return current_process->pid;
}
