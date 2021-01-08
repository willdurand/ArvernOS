#include "process.h"
#include "logging.h"
#include <core/isr.h>
#include <core/register.h>
#include <drivers/timer.h>
#include <proc/spinlock.h>
#include <proc/usermode.h>
#include <stdlib.h>
#include <string.h>

process_t* process_create_root();

static process_t* current_process = NULL;

static pcb_t* current_task = NULL;
static MAKE_LOCK(scheduler);

extern void switch_tasks(pcb_t* old_task, pcb_t* new_task);

void process_init()
{
  pcb_t* main = (pcb_t*)calloc(1, sizeof(pcb_t));
  main->rsp = read_rsp();
  main->cr3 = read_cr3();
  main->next = NULL;
  strcpy(main->name, "init");

  LOCK(scheduler);
  current_task = main;
  UNLOCK(scheduler);

  PROC_DEBUG(
    "creating task '%s': rsp=%p cr3=%p", main->name, main->rsp, main->cr3);
}

void process_yield()
{
  if (current_task == NULL) {
    return;
  }

  if (current_task->next == NULL) {
    PROC_DEBUG("running: %s", current_task->name);
    return;
  }

  LOCK(scheduler);

  // Round robin!
  pcb_t *new_task, *old_task;
  new_task = current_task->next;
  old_task = current_task;
  current_task = new_task;
  PROC_DEBUG("current task is now '%s'", current_task->name);

  pcb_t* t = current_task;
  while (t->next != NULL) {
    t = t->next;
  }
  old_task->next = NULL;
  t->next = old_task;
  PROC_DEBUG("adding task '%s' after task '%s'", old_task->name, t->name);

  UNLOCK(scheduler);

  PROC_DEBUG(
    "switching from task '%s' (rsp=%p cr3=%p) to task '%s' (rsp=%p cr3=%p)",
    old_task->name,
    old_task->rsp,
    old_task->cr3,
    new_task->name,
    new_task->rsp,
    new_task->cr3);

  switch_tasks(old_task, new_task);
}

void process_create_task(const char* name, uint64_t entrypoint)
{
  pcb_t* task = (pcb_t*)calloc(1, sizeof(pcb_t));
  void* stack = (void*)&task->stack[STACK_SIZE - 1];
  PUSH_TO_STACK(stack, uint64_t, entrypoint);
  PUSH_TO_STACK(stack, uint64_t, 0);
  PUSH_TO_STACK(stack, uint64_t, 0);
  task->rsp = (uint64_t)stack;
  task->cr3 = read_cr3();
  task->next = NULL;
  strcpy(task->name, name);

  LOCK(scheduler);
  pcb_t* t = current_task;
  while (t->next != NULL) {
    t = t->next;
  }
  t->next = task;
  UNLOCK(scheduler);

  PROC_DEBUG("adding task '%s' after task '%s': rsp=%p cr3=%p entrypoint=%p",
             task->name,
             t->name,
             task->rsp,
             task->cr3,
             entrypoint);
}

process_t* process_create_root()
{
  current_process = (process_t*)malloc(sizeof(process_t));
  current_process->pid = 0;

  return current_process;
}

process_t* process_get_current()
{
  return current_process;
}

process_t* process_exec(uint8_t* image, const char* name, char* const argv[])
{
  PROC_DEBUG("image=%p name=%s", image, name);

  elf_header_t* old_elf = NULL;

  if (current_process == NULL) {
    current_process = process_create_root();
  } else {
    // Give a new PID to the current process.
    current_process->pid++;
    // Save the old ELF pointer for later.
    old_elf = current_process->elf;
    // Free the current name as we'll update it right after.
    free(current_process->name);
    // Free old args.
    for (int i = 0; current_process->argv[i] != NULL; i++) {
      free(current_process->argv[i]);
    }
    free(current_process->argv);
    // Free old env vars.
    for (int i = 0; current_process->envp[i] != NULL; i++) {
      free(current_process->envp[i]);
    }
    free(current_process->envp);
  }

  // Set current process name.
  current_process->name = strdup(name);
  // Set up user stack.
  memset(current_process->user_stack, 0, STACK_SIZE);

  // We need both `argc` and `argv` so we start by retrieving `argc`. Then, we
  // need to `strdup()` all the given arguments becaue `exec` replaces the
  // (old) process image so we won't have access to them for a very long time.
  int argc = 0;
  while (argv[argc]) {
    argc++;
  }

  char** _argv = (char**)malloc(sizeof(char*) * (argc + 1));
  for (int i = 0; i < argc; i++) {
    _argv[i] = strdup(argv[i]);
  }
  _argv[argc] = NULL;
  current_process->argv = _argv;

  // TODO: do not use a fixed size.
  char** envp = (char**)calloc(1, sizeof(char*) * 10);
  current_process->envp = envp;

  void* stack = (void*)&current_process->user_stack[STACK_SIZE - 1];
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)envp);
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)_argv);
  PUSH_TO_STACK(stack, uint64_t, (uint64_t)argc);
  current_process->user_rsp = (uint64_t)stack;

  if (old_elf) {
    // Unload current process.
    elf_unload(old_elf);
    free(old_elf);
  }

  // Load ELF in current process.
  elf_header_t* elf = elf_load(image);
  current_process->elf = elf;

  return current_process;
}
