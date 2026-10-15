#include <proc/task.h>

#include <arch/proc.h>
#include <arvern/utils.h>
#include <assert.h>
#include <core/isr.h>
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

static void task_set_name(task_t* task, const char* name)
{
  if (name == NULL) {
    task->name[0] = '\0';
    return;
  }

  strncpy(task->name, name, TASK_NAME_LEN - 1);
  task->name[TASK_NAME_LEN - 1] = '\0';
}

void task_init(const char* init_name)
{
  static_assert(offsetof(task_t, info) == 0,
                "task_info_t should be the first member of task_t");

  INFO("%s", "proc: initialize scheduler");

  task_t* init_task = arch_task_init();
  if (init_task == NULL) {
    return;
  }

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
  tasks[current->id]->state = ZOMBIE;
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
  tasks[task->id] = task;

  task_preempt_enable();

  return 0;
}

void task_do_schedule()
{
  task_preempt_disable();

  uint32_t next;
  int64_t c;
  task_t* p;

  while (1) {
    c = -1;
    next = 0;

    for (int i = 0; i < MAX_TASKS; i++) {
      p = tasks[i];
      if (p && p->state == RUNNING && p->counter > c) {
        c = p->counter;
        next = i;
      }
    }

    if (c) {
      break;
    }

    for (int i = 0; i < MAX_TASKS; i++) {
      p = tasks[i];

      if (p) {
        p->counter = (p->counter >> 1) + p->priority;
      }
    }
  }

  task_switch(tasks[next]);

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
