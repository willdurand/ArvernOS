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
extern void ret_from_fork_user();
extern uintptr_t* syscall_frame;
extern void switch_to_ring3(uintptr_t entry,
                            uintptr_t user_stack_top,
                            uintptr_t kernel_stack_top);

// Number of values pushed by the syscall entry stub: r15..rbp, rip, rflags and
// the user stack pointer (see `k_syscall.asm`).
#define SYSCALL_FRAME_SLOTS 15

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
  -1,          // parent pid
  0,           // exit code
  -1,          // pid being waited for
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

task_t* arch_task_create_child(task_t* parent)
{
  if (parent == NULL || syscall_frame == NULL) {
    return NULL;
  }

  task_t* p = (task_t*)valloc(PAGE_SIZE);
  if (!p) {
    return NULL;
  }

  memset((void*)&p->info, 0, sizeof(task_info_t));

  p->flags = parent->flags & ~KERNEL_TASK;
  p->priority = parent->priority;
  p->state = RUNNING;
  p->counter = p->priority;
  p->preempt_count = 1;

  uintptr_t kernel_stack_top = (uintptr_t)p + PAGE_SIZE;

  // Copy the parent's saved user context (syscall frame) to the very top of the
  // child's kernel stack. `ret_to_user` will pop it and `sysret` back to user
  // mode exactly where the parent was when it called `fork()`.
  uintptr_t* frame =
    (uintptr_t*)(kernel_stack_top - SYSCALL_FRAME_SLOTS * sizeof(uintptr_t));
  for (int i = 0; i < SYSCALL_FRAME_SLOTS; i++) {
    frame[i] = syscall_frame[i];
  }

  // Below the syscall frame, seed the same context that `arch_task_switch()`
  // expects (rflags, r15..rbp) plus a return address pointing at
  // `ret_from_fork_user()`, which resumes the copied syscall frame with
  // `rax=0`.
  uintptr_t* rsp = (uintptr_t*)frame;
  *--rsp = (uintptr_t)ret_from_fork_user;
  *--rsp = 0;     // rbp
  *--rsp = 0;     // rbx
  *--rsp = 0;     // r12
  *--rsp = 0;     // r13
  *--rsp = 0;     // r14
  *--rsp = 0;     // r15
  *--rsp = 0x202; // rflags (IF)

  p->info.rsp = (uintptr_t)rsp;
  p->kernel_stack_top = kernel_stack_top;
  // Eagerly copy the parent's user pages into a private address space so that
  // the parent and child do not share their user stack.
  p->address_space = paging_clone_user();
  if (p->address_space == NULL) {
    free(p);
    return NULL;
  }

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
