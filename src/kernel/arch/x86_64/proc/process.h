#ifndef PROC_PROCESS_H
#define PROC_PROCESS_H

#include <core/elf.h>
#include <sys/types.h>

#define USER_STACK_SIZE 0x3000
#define USER_STACK_TOP  0x1000
#define USER_STACK_BUF  0x2000

typedef struct process
{
  pid_t pid;
  char* name;
  elf_header_t* elf;
  uint64_t user_stack[USER_STACK_SIZE];
  uint64_t user_rsp;
  // TODO: We should probably retain the file descriptors that are opened by
  // the process so that we can close them.
} process_t;

process_t* process_create_root();

process_t* process_exec(uint8_t* image, const char* name, char* const argv[]);

process_t* process_get_current();

#endif
