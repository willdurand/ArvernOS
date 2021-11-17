#include <arch/panic.h>

#include <logging.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stack_frame
{
  struct stack_frame* rbp;
  uint64_t rip;
} stack_frame_t;

char* kernel_find_symbol(uint64_t* addr);

static char* symbols = NULL;

void arch_kernel_load_symbols(uint64_t addr, uint64_t size)
{
  symbols = (char*)malloc(sizeof(char) * (size + 1));
  memcpy(symbols, (void*)addr, size);
  symbols[size] = 0;
}

void arch_kernel_dump_stacktrace()
{
  printf("\n  Kernel stacktrace:\n\n");
  DEBUG("%s", "kernel stacktrace:");

  stack_frame_t* stackframe = NULL;
  __asm__("movq %%rbp, %0" : "=r"(stackframe));

  while (stackframe != NULL) {
    uint64_t address = stackframe->rip;
    uint64_t symbol_addr = address;

    char* symbol = kernel_find_symbol(&symbol_addr);
    char* name = symbol ? symbol : "???";
    uint64_t offset = symbol ? (address - symbol_addr) : 0;

    printf("     %p - %s+0x%x\n", address, name, offset);
    DEBUG("  %p - %s+0x%x", address, name, offset);

    if (symbol) {
      free(symbol);
    }

    stackframe = stackframe->rbp;
  }
}

char* kernel_find_symbol(uint64_t* addr)
{
  char* retval = NULL;

  char* line = symbols;
  uint64_t size = strlen(symbols);

  uint64_t prev_symbol_addr = 0;
  char* prev_symbol_name = NULL;
  char* symbol_name = NULL;

  // Linear lookup, not great (as in not efficient) but it works.
  while (line && line < (char*)symbols + size) {
    uint64_t symbol_addr = strtol_wip(line, &symbol_name, 16);
    symbol_name += 1;

    if (!prev_symbol_addr) {
      prev_symbol_addr = symbol_addr;
      prev_symbol_name = symbol_name;
    }

    if (symbol_addr > *addr) {
      if (prev_symbol_name != NULL) {
        uint8_t len =
          (uint8_t)(strchrnul(prev_symbol_name, '\n') - prev_symbol_name) + 1;
        retval = (char*)malloc(sizeof(char) * (len + 1));
        strncpy(retval, prev_symbol_name, len);
        *addr = prev_symbol_addr;
      }

      break;
    }

    prev_symbol_addr = symbol_addr;
    prev_symbol_name = symbol_name;
    line = strchr(line, '\n') + 1;
  }

  return retval;
}
