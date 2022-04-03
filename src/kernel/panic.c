#include <panic.h>

#include <arch/kernel.h>
#include <fcntl.h>
#include <initcall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/k_syscall.h>

#define STACKTRACE_MAX 16

static char* kernel_find_symbol(uintptr_t* addr);

static bool panicked = false;
static char* symbols = NULL;

typedef struct stack_frame
{
  struct stack_frame* next_frame;
  uintptr_t return_address;
} stack_frame_t;

void kernel_panic(bool dump_stacktrace, const char* format, ...)
{
  if (panicked) {
    arch_halt();
  }

  panicked = true;

  printf("\033[0;31m");

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  if (dump_stacktrace) {
    kernel_dump_stacktrace();
  }

  printf("\n\n%45s\033[0m", "SYSTEM HALTED!");

  arch_halt();
}

void kernel_dump_stacktrace()
{
  printf("\n  Kernel stacktrace:\n\n");
#ifdef __x86_64__
  DEBUG("%s", "kernel stacktrace:");
#endif

  stack_frame_t* stackframe = __builtin_frame_address(0);

  uint8_t i = 0;
  while (stackframe != NULL && i++ < STACKTRACE_MAX) {
    uintptr_t address = stackframe->return_address;
    uintptr_t symbol_addr = address;

    if (!arch_is_kernel_address(address)) {
      break;
    }

    char* symbol = kernel_find_symbol(&symbol_addr);
    char* name = symbol ? symbol : "???";
    uint64_t offset = symbol ? (address - symbol_addr) : 0;

    printf("     %p - %s+0x%" PRIx64 "\n", address, name, offset);
#ifdef __x86_64__
    DEBUG("  %p - %s+0x%" PRIx64, address, name, offset);
#endif

    if (symbol) {
      free(symbol);
    }

    stackframe = stackframe->next_frame;
  }
}

static char* kernel_find_symbol(uintptr_t* addr)
{
  if (symbols == NULL) {
    return NULL;
  }

  char* retval = NULL;

  char* line = symbols;
  uint64_t size = strlen(symbols);

  uint64_t prev_symbol_addr = 0;
  char* prev_symbol_name = NULL;
  char* symbol_name = NULL;

  // Linear lookup, not great (as in not efficient) but it works.
  while (line && line < (char*)symbols + size) {
    uintptr_t symbol_addr = strtol_wip(line, &symbol_name, 16);
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

    line = strchr(line, '\n');
    if (line) {
      line += 1;
    }
  }

  return retval;
}

static int kernel_symbols_init()
{
  DEBUG("%s", "loading debug symbols");

  int fd = k_open("/etc/symbols.txt", O_RDONLY);
  if (fd < 0) {
    return -1;
  }

  struct stat stat_buf;
  if (k_fstat(fd, &stat_buf) < 0) {
    return -2;
  }

  symbols = (char*)malloc((stat_buf.st_size + 1) * sizeof(char));
  if (symbols == NULL) {
    return -3;
  }

  if (k_read(fd, symbols, stat_buf.st_size) != stat_buf.st_size) {
    return -4;
  }

  symbols[stat_buf.st_size] = 0;

  if (k_close(fd) < 0) {
    return -5;
  }

  return 0;
}

initcall_early_register(kernel_symbols_init);
