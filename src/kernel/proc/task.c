#include <proc/task.h>

#include <arch/proc.h>

#if defined(__x86_64__)
#include <mmu/paging.h>
#endif

#include <arvern/utils.h>
#include <assert.h>
#include <core/isr.h>
#include <errno.h>
#include <mmu/page.h>
#include <proc/logging.h>
#include <stdlib.h>
#include <string.h>

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
  init_task->parent = -1;
  init_task->wait_pid = -1;

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

  task_preempt_disable();
  current->state = ZOMBIE;
  current->exit_code = code;

  PROC_DEBUG(
    "task_exit: task=%d name=%s code=%d", current->id, current->name, code);

  task_t* parent = NULL;
  if (current->parent >= 0 && current->parent < MAX_TASKS) {
    parent = tasks[current->parent];
  }

  if (parent != NULL && parent->state == WAITING) {
    if (parent->wait_pid == current->id || parent->wait_pid == -1 ||
        parent->wait_pid == 0) {
      parent->state = RUNNING;
    }
  }

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
  task->parent = (current != NULL) ? current->id : -1;
  task->exit_code = 0;
  task->wait_pid = -1;

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

static task_t* task_find_zombie_child(task_t* parent, pid_t pid)
{
  for (int i = 0; i < MAX_TASKS; i++) {
    task_t* task = tasks[i];

    if (task == NULL || task->parent != parent->id || task->state != ZOMBIE) {
      continue;
    }

    if (pid > 0 && task->id != pid) {
      continue;
    }

    return task;
  }

  return NULL;
}

static void task_reap(task_t* task)
{
  tasks[task->id] = NULL;

#if defined(__x86_64__)
  if (task->address_space != NULL) {
    paging_deactivate(task->address_space);
  }
#endif

  free(task);
}

pid_t task_wait(pid_t pid, int* status)
{
  task_t* parent = current;

  if (parent == NULL) {
    return -ECHILD;
  }

  bool found = false;

  for (int i = 0; i < MAX_TASKS; i++) {
    if (tasks[i] != NULL && tasks[i]->parent == parent->id &&
        (pid <= 0 || tasks[i]->id == pid)) {
      found = true;
      break;
    }
  }

  if (!found) {
    return -ECHILD;
  }

  for (;;) {
    task_preempt_disable();

    task_t* child = task_find_zombie_child(parent, pid);

    if (child == NULL) {
      parent->state = WAITING;
      parent->wait_pid = pid;
      task_preempt_enable();

      task_schedule();
      continue;
    }

    pid_t id = child->id;

    if (status != NULL) {
      *status = child->exit_code;
    }

    task_reap(child);
    task_preempt_enable();

    return id;
  }
}

task_t* task_fork_child(task_t* parent)
{
  if (parent == NULL) {
    return NULL;
  }

  task_preempt_disable();

  int free_id = -1;
  for (int i = 0; i < MAX_TASKS; i++) {
    if (tasks[i] == NULL) {
      free_id = i;
      break;
    }
  }
  if (free_id < 0) {
    task_preempt_enable();
    return NULL;
  }

  // This allocates the child task (and its kernel stack) with a copy of the
  // parent's saved user context already seeded on it.
  task_t* child = arch_task_create_child(parent);
  if (child == NULL) {
    task_preempt_enable();
    return NULL;
  }

  child->id = free_id;
  child->parent = parent->id;
  child->exit_code = 0;
  child->wait_pid = -1;
  child->user_stack_base = parent->user_stack_base;
  tasks[free_id] = child;

  strncpy(child->name, parent->name, TASK_NAME_LEN - 1);
  child->name[TASK_NAME_LEN - 1] = '\0';

  task_preempt_enable();

  return child;
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

    char state = tasks[i]->state == RUNNING   ? 'R'
                 : tasks[i]->state == WAITING ? 'W'
                                              : 'Z';
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
