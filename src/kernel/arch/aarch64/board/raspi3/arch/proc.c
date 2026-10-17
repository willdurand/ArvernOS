#include <arch/proc.h>

#include <arch/task.h>
#include <arvern/utils.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4096

extern void ret_from_fork();
extern void switch_to_el0(uintptr_t entry, uintptr_t user_stack_top);

static task_t init_task = (task_t){
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // task_info_t
  0,                                         // task ID
  "",                                        // name
  RUNNING,                                   // state
  1,                                         // counter
  1,                                         // priority
  0,                                         // preempt_count
  KERNEL_TASK,                               // flags
  0,                                         // kernel_stack_top
  NULL,                                      // address_space
  0,                                         // user_stack_base
};

task_t* arch_task_init()
{
  return &init_task;
}

task_t* arch_task_create(uint64_t clone_flags, uintptr_t entrypoint)
{
  task_t* p = (task_t*)valloc(PAGE_SIZE);

  if (!p) {
    return NULL;
  }

  memset((void*)&p->info, 0, sizeof(task_info_t));

  p->flags = clone_flags;
  p->priority = CURRENT_TASK->priority;
  p->state = RUNNING;
  p->counter = p->priority;
  // disable preemption until task_schedule_tail()
  p->preempt_count = 1;

  p->info.x19 = entrypoint;
  p->info.pc = (uintptr_t)ret_from_fork;
  p->info.sp = (uintptr_t)p + PAGE_SIZE;

  return p;
}

__attribute__((noreturn)) void arch_task_enter_user(uintptr_t entry,
                                                    uintptr_t user_stack_top,
                                                    task_t* task)
{
  UNUSED(task);

  switch_to_el0(entry, user_stack_top);

  __builtin_unreachable();
}

void arch_task_set_kernel_stack(task_t* task)
{
  UNUSED(task);
}
