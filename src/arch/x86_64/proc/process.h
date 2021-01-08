#ifndef PROC_PROCESS_H
#define PROC_PROCESS_H

#include <core/elf.h>
#include <sys/types.h>

#define STACK_SIZE 1024

struct pcb;

/// This structure represents a Process Control Block.
typedef struct pcb
{
  uint64_t rsp;
  uint64_t cr3;

  char name[20];
  uint64_t stack[STACK_SIZE];

  struct pcb* next;
} pcb_t;

typedef struct process
{
  pid_t pid;
  char* name;
  elf_header_t* elf;
  char** argv;
  char** envp;
  uint64_t user_stack[STACK_SIZE];
  uint64_t user_rsp;
  // TODO: we should probably retain the file descriptors that are opened by
  // the process so that we can close them.
} process_t;

/**
 * Initializes the process scheduler.
 */
void process_init();

/**
 * Gives CPU to another process.
 */
void process_yield();

void process_create_task(const char* name, uint64_t entrypoint);

process_t* process_exec(uint8_t* image, const char* name, char* const argv[]);

process_t* process_get_current();

#endif
