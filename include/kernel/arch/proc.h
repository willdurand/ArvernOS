/**
 * @file
 */
#ifndef ARCH_PROC_H
#define ARCH_PROC_H

#include <proc/task.h>

/**
 * Size (in bytes) of the user stack that `k_execv()` prepares for a user
 * program. On architectures with a memory management unit, the whole stack
 * has to fit in this range so that it can be mapped as user-accessible.
 */
#define USER_STACK_SIZE (32 * 1024)

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

/**
 * Enters user mode and starts executing at `entry`.
 *
 * This function is called by `k_execv()` and it does not return.
 *
 * @param entry the user mode entry point
 * @param user_stack_top the top (high address) of the user stack
 * @param task the task entering user mode
 */
void arch_task_enter_user(uintptr_t entry,
                          uintptr_t user_stack_top,
                          task_t* task);

#endif
