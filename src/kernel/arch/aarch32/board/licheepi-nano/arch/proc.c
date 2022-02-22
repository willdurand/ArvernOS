#include <arch/proc.h>

#include <arch/task.h>
#include <arvern/utils.h>

static task_t init_task = (task_t){
  {},         // task_info_t
  0,          // task ID
  "",         // name
  RUNNING,    // state
  0,          // counter
  1,          // priority
  0,          // preempt_count
  PT_KTHREAD, // flags
};

task_t* arch_task_init()
{
  return &init_task;
}

task_t* arch_task_create(uint64_t clone_flags, uintptr_t entrypoint)
{
  UNUSED(clone_flags);
  UNUSED(entrypoint);

  // TODO: Implement this function for Lichee Pi Nano.

  return NULL;
}

void arch_task_switch(task_t* prev, task_t* next)
{
  UNUSED(*prev);
  UNUSED(*next);
}
