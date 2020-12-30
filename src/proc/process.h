#ifndef PROC_PROCESS_H
#define PROC_PROCESS_H

#include <core/elf.h>
#include <sys/types.h>

typedef struct process
{
  pid_t pid;
  char* name;
  elf_header_t* elf;
  char** argv;
  uint64_t user_stack[1024];
} process_t;

#endif
