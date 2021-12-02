/** @file */
#ifndef PROC_SPINLOCK_H
#define PROC_SPINLOCK_H

#define MAKE_LOCK(name) volatile int name##_lock

#define LOCK(name)                                                             \
  while (!__sync_bool_compare_and_swap(&name##_lock, 0, 1)) {                  \
    ;                                                                          \
  }                                                                            \
  __sync_synchronize();

#define UNLOCK(name)                                                           \
  __sync_synchronize();                                                        \
  name##_lock = 0;

#endif
