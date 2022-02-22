#include <arch/proc.h>

#include <arch/task.h>
#include <string.h>

#define PAGE_SIZE 4096

extern void ret_from_fork();

static task_t init_task = (task_t){
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // task_info_t
  0,                                   // task ID
  "",                                  // name
  RUNNING,                             // state
  0,                                   // counter
  1,                                   // priority
  0,                                   // preempt_count
  PT_KTHREAD,                          // flags
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
  // disable preemtion until task_schedule_tail()
  p->preempt_count = 1;

  p->info.r4 = entrypoint;
  p->info.cpsr = 0x13 | (8 << 1); // svc + irq enabled
  p->info.sp = (uintptr_t)p + PAGE_SIZE;
  p->info.pc = (uintptr_t)ret_from_fork;

  return p;
}
