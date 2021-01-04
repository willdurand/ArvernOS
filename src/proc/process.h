#ifndef PROC_PROCESS_H
#define PROC_PROCESS_H

#include <core/elf.h>
#include <sys/types.h>

#define USER_STACK_SIZE 1024

typedef struct process
{
  pid_t pid;
  char* name;
  elf_header_t* elf;
  char** argv;
  char** envp;
  void* user_stack;
  uint64_t user_rsp;
  // TODO: we should probably retain the file descriptors that are opened by
  // the process so that we can close them.
} process_t;

process_t* process_create_root();

process_t* process_exec(uint8_t* image, const char* name, char* const argv[]);

process_t* process_get_current();

#endif
