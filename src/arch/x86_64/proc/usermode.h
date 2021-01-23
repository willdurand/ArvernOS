#ifndef PROC_USERMODE_H
#define PROC_USERMODE_H

#define PUSH_TO_STACK(stack, type, value)                                      \
  stack = (char*)stack - sizeof(type);                                         \
  *((type*)stack) = value

extern void switch_to_usermode(void* instr_ptr, void* stack_ptr);

#endif
