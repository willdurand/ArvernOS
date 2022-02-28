#include <arch/proc.h>

#include <arch/task.h>
#include <core/gdt.h>
#include <logging.h>
#include <mmu/frame.h>
#include <stdlib.h>
#include <string.h>

extern void ret_from_fork();

static task_t init_task = (task_t){
  { 0 },       // task_info_t
  0,           // task ID
  "",          // name
  RUNNING,     // state
  0,           // counter
  1,           // priority
  0,           // preempt_count
  KERNEL_TASK, // flags
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

  uintptr_t* rsp = (uintptr_t*)((uintptr_t)p + PAGE_SIZE);

  *--rsp = (uintptr_t)entrypoint;
  *--rsp = (uintptr_t)ret_from_fork; // This will be used in `arch_task_switch`
                                     // to call `ret_from_fork`.
  *--rsp = 0;                        // rbp
  *--rsp = 0;                        // rbx
  *--rsp = 0;                        // r12
  *--rsp = 0;                        // r13
  *--rsp = 0;                        // r14
  *--rsp = 0;                        // r15
  *--rsp = 0x202;                    // cpu flags

  p->info.rsp = (uintptr_t)rsp;

  return p;
}
