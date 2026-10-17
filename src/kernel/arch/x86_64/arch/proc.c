#include <arch/proc.h>

#include <arch/task.h>
#include <arvern/utils.h>
#include <core/gdt.h>
#include <core/tss.h>
#include <logging.h>
#include <mmu/frame.h>
#include <mmu/paging.h>
#include <stdlib.h>
#include <string.h>

extern void ret_from_fork();
extern void switch_to_ring3(uintptr_t entry,
                            uintptr_t user_stack_top,
                            uintptr_t kernel_stack_top);

static task_t init_task = (task_t){
  { 0 },       // task_info_t
  0,           // task ID
  "",          // name
  RUNNING,     // state
  1,           // counter
  1,           // priority
  0,           // preempt_count
  KERNEL_TASK, // flags
  0,           // kernel_stack_top
  NULL,        // address_space
  0,           // user_stack_base
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
  p->kernel_stack_top = (uintptr_t)p + PAGE_SIZE;
  p->address_space = NULL;
  p->user_stack_base = 0;

  return p;
}

__attribute__((noreturn)) void arch_task_enter_user(uintptr_t entry,
                                                    uintptr_t user_stack_top,
                                                    task_t* task)
{
  // Reset the kernel stack to the top of this task's stack page, then let the
  // assembly build the `iretq` frame that drops us to ring 3.
  uintptr_t kernel_stack_top = (task != NULL) ? task->kernel_stack_top : 0;

  switch_to_ring3(entry, user_stack_top, kernel_stack_top);

  __builtin_unreachable();
}

void arch_task_set_kernel_stack(task_t* task)
{
  if (task == NULL) {
    return;
  }

  tss_set_rsp0(task->kernel_stack_top);
}
