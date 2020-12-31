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
  uint64_t user_rsp;
} process_t;

process_t* process_create_root();

process_t* process_exec(uint8_t* image, const char* name, char* const argv[]);

process_t* process_get_current();

#endif
