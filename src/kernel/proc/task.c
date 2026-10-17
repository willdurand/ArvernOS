#include <proc/task.h>

#include <arch/proc.h>

#if defined(__x86_64__)
#include <mmu/paging.h>
#endif

#include <arvern/utils.h>
#include <assert.h>
#include <core/isr.h>
#include <proc/logging.h>
#include <stdlib.h>
#include <string.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

void task_do_schedule();
void task_preempt_disable();
void task_preempt_enable();
void task_schedule_tail();
void task_switch(task_t* next);

static task_t* current = NULL;

static task_t* tasks[MAX_TASKS] = { NULL };

void task_init(const char* init_name)
{
  static_assert(offsetof(task_t, info) == 0,
                "task_info_t should be the first member of task_t");

  INFO("%s", "proc: initialize scheduler");

  task_t* init_task = arch_task_init();
  if (init_task == NULL) {
    return;
  }

  // `init_task` is statically initialised by arch_task_init() (including
  // `kernel_stack_top` and `address_space`), so task_init() only overrides
  // the fields it owns.
  task_set_name(init_task, init_name);

  assert(init_task->id >= 0 && init_task->id < MAX_TASKS);

  current = init_task;
  tasks[init_task->id] = init_task;
}

void task_schedule()
{
  if (current == NULL) {
    return;
  }
  current->counter = 0;
  task_do_schedule();
}

void task_schedule_on_timer_tick()
{
  if (!current) {
    return;
  }

  isr_disable_interrupts();
  current->counter--;

  if (current->counter > 0 || current->preempt_count > 0) {
    isr_enable_interrupts();
    return;
  }

  current->counter = 0;
  isr_enable_interrupts();

  task_do_schedule();
}

void task_schedule_tail()
{
  task_preempt_enable();
}

void task_exit(int code)
{
  if (current == NULL) {
    return;
  }

  // TODO: set the exit code to the zombie task.
  UNUSED(code);

  task_preempt_disable();
  current->state = ZOMBIE;
  task_preempt_enable();

  task_schedule();
}

task_t* task_get_current()
{
  return current;
}

int task_create(const char* name, uint64_t clone_flags, uintptr_t entrypoint)
{
  task_preempt_disable();

  task_t* task = arch_task_create(clone_flags, entrypoint);

  if (task == NULL) {
    task_preempt_enable();
    return -1;
  }

  int free_id = -1;
  for (int i = 0; i < MAX_TASKS; i++) {
    if (tasks[i] == NULL) {
      free_id = i;
      break;
    }
  }

  if (free_id < 0) {
    task_preempt_enable();
    return -1;
  }

  task->id = free_id;
  task_set_name(task, name);

  if (task->id < MAX_TASKS) {
    tasks[task->id] = task;
  } else {
    task_preempt_enable();
    return -1;
  }

  if (task->kernel_stack_top == 0) {
    task->kernel_stack_top = (uintptr_t)task + PAGE_SIZE;
  }

  task_preempt_enable();

  return task->id;
}

void task_do_schedule()
{
  task_preempt_disable();

  uint32_t next = 0;
  int64_t c;
  task_t* p;
  bool found_runnable = false;

  while (1) {
    c = -1;
    next = 0;
    found_runnable = false;

    for (int i = 0; i < MAX_TASKS; i++) {
      p = tasks[i];
      if (p && p->state == RUNNING && p->counter > c) {
        c = p->counter;
        next = i;
        found_runnable = true;
      }
    }

    // Nothing runnable: there is no task to switch to.
    if (!found_runnable) {
      task_preempt_enable();
      return;
    }

    // Only break once we found a runnable task that still has some time slice
    // left. A counter of 0 means the task used its quantum, in which case we
    // need to replenish the counters of every runnable task (including this
    // one) and pick again. Breaking on `c == 0` would let the task with the
    // lowest slot index (usually `idle`) win every subsequent tie and starve
    // the other tasks forever.
    if (c > 0) {
      break;
    }

    for (int i = 0; i < MAX_TASKS; i++) {
      p = tasks[i];

      if (p && p->state == RUNNING) {
        p->counter = (p->counter >> 1) + p->priority;
      }
    }
  }

  if (next < MAX_TASKS && tasks[next]) {
    task_switch(tasks[next]);
  }

  task_preempt_enable();
}

void task_preempt_disable()
{
  if (current == NULL) {
    return;
  }

  current->preempt_count++;
}

void task_preempt_enable()
{
  if (current == NULL) {
    return;
  }

  if (current->preempt_count > 0) {
    current->preempt_count--;
  }
}

void task_switch(task_t* next)
{
  if (current == next) {
    return;
  }

  PROC_DEBUG("%s -> %s", current->name, next->name);

  task_t* prev = current;
  current = next;

  // Make sure any interrupt/syscall entry from `next` lands on its own kernel
  // stack.
  arch_task_set_kernel_stack(next);

#if defined(__x86_64__)
  // Switch to the address space of the task that is about to run. A `NULL`
  // address space means the kernel page table.
  paging_activate(next->address_space);
#endif

  arch_task_switch(prev, next);
}

void task_print_info()
{
  printf("state name\n");
  printf("----- ----\n");

  for (uint8_t i = 0; i < MAX_TASKS; i++) {
    if (tasks[i] == NULL) {
      continue;
    }

    char state = tasks[i]->state == RUNNING ? 'R' : 'Z';
    printf("%c     %s\n", state, tasks[i]->name);
  }
}

void task_set_name(task_t* task, const char* name)
{
  if (name == NULL) {
    task->name[0] = '\0';
    return;
  }

  strncpy(task->name, name, TASK_NAME_LEN - 1);
  task->name[TASK_NAME_LEN - 1] = '\0';
}
