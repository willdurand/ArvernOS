/**
 * @file
 *
 * This module implements multi-tasking and it is based on Sergey Matyukevich's
 * work (specifically the scheduler implemented in RPi OS), released under a
 * MIT license. See:
 * https://s-matyukevich.github.io/raspberry-pi-os/docs/lesson04/rpi-os.html
 */
#ifndef PROC_TASK_H
#define PROC_TASK_H

#include <arch/task.h>
#include <stdint.h>

// TODO: Get rid of this limitation.
#define MAX_TASKS 64

#define KERNEL_TASK 0x01

/// This special variable points to the current task being executed.
#define CURRENT_TASK (task_get_current())

typedef enum task_state
{
  RUNNING,
  ZOMBIE,
} task_state_t;

// Forward declaration. This should be implemented in the architecture-specific
// code, which is expected to be found in `arch/task.h`.
typedef struct task_info task_info_t;

typedef struct task
{
  // This should be the first field in this structure because it is easier to
  // access in assembly code.
  task_info_t info;
  uint32_t id;
  char name[20];
  task_state_t state;
  int64_t counter;
  uint64_t priority;
  int64_t preempt_count;
  uint64_t flags;
} task_t;

/**
 * Initializes the scheduler.
 */
void task_init();

/**
 * Schedules a new task.
 */
void task_schedule();

/**
 * Schedules a new task.
 *
 * This is similar to `task_schedule()` but designed to be automatically called
 * at regular interval.
 */
void task_schedule_on_timer_tick();

/**
 * Terminates the current task.
 *
 * @param code the status code
 */
void task_exit(int code);

/**
 * Returns the current task.
 *
 * @return the current task if it exists, `NULL` otherwise
 */
task_t* task_get_current();

/**
 * Creates a new task.
 *
 * @param name the name of the task to create
 * @param clone_flags configuration flags
 * @param entrypoint the entrypoint of the task
 * @return `0` on success, `-1` otherwise
 */
int task_create(const char* name, uint64_t clone_flags, uintptr_t entrypoint);

/**
 * Prints information about the tasks on the standard output.
 */
void task_print_info();

#endif
