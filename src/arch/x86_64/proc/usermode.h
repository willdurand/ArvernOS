#ifndef PROC_USERMODE_H
#define PROC_USERMODE_H

#define PUSH_TO_STACK(stack, type, value)                                      \
  stack = stack - sizeof(type);                                                \
  /* Make sure that the stack is aligned. */                                   \
  while ((uintptr_t)stack & (sizeof(type) - 1)) {                              \
    stack--;                                                                   \
  }                                                                            \
  *((type*)stack) = value

#define PUSHSTR_TO_STACK(stack, str)                                           \
  do {                                                                         \
    ssize_t len = strlen(str);                                                 \
    do {                                                                       \
      PUSH_TO_STACK(stack, char, str[len]);                                    \
      len--;                                                                   \
    } while (len >= 0);                                                        \
  } while (0)

extern void switch_to_usermode(void* instr_ptr, void* stack_ptr);

#endif
