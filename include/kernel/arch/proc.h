/**
 * @file
 */
#ifndef ARCH_PROC_H
#define ARCH_PROC_H

#include <proc/task.h>

/**
 * Creates and returns the initial task.
 *
 * This function is called by `task_init()`.
 *
 * @return the initial task
 */
task_t* arch_task_init();

/**
 * Switches to a new task.
 *
 * This function is called by `task_switch()`, which is called by
 * `task_schedule()` and `task_schedule_on_timer_tick()`.
 *
 * @param prev the task that should be stopped
 * @param next the task that should be started
 */
void arch_task_switch(task_t* prev, task_t* next);

/**
 * Creates a new task.
 *
 * This function is called by `task_create()`.
 *
 * @param clone_flags configuration flags
 * @param entrypoint the entrypoint of the task
 * @return a task
 */
task_t* arch_task_create(uint64_t clone_flags, uintptr_t entrypoint);

#endif
