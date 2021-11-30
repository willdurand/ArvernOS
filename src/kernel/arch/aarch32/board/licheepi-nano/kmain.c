#include "kmain.h"
#include <drivers/clock.h>
#include <drivers/uart0.h>
#include <kmain.h>
#include <mmu/alloc.h>
#include <stdio.h>
#include <sys/k_syscall.h>

#define ATAG_NONE      0
#define ATAG_CORE      0x54410001
#define ATAG_MEM       0x54410002
#define ATAG_VIDEOTEXT 0x54410003
#define ATAG_RAMDISK   0x54410004
#define ATAG_INITRD2   0x54420005
#define ATAG_SERIAL    0x54410006
#define ATAG_REVISION  0x54410007
#define ATAG_VIDEOLFB  0x54410008
#define ATAG_CMDLINE   0x54410009

typedef struct atag_header
{
  uint32_t size;
  uint32_t tag;
} atag_header_t;

typedef struct atag_mem
{
  atag_header_t header;
  uint32_t size;
  uint32_t start;
} atag_mem_t;

typedef struct atag_cmdline
{
  atag_header_t header;
  char cmdline[];
} atag_cmdline_t;

extern unsigned char __bss_end;
extern unsigned char __bss_start;
extern unsigned char __heap_start;
extern unsigned char __stack_top;
extern unsigned char __initcall_start;
extern unsigned char __initcall_end;

void kmain(uint32_t r0, uint32_t r1, uint32_t r2)
{
  clock_init();
  uart0_init();

  // TODO: move that elsewhere, possibly in a `TRACE()` call, which we don't
  // have currently. Maybe use logging v2?
  printf("r0=%p r1=%p r2=%p\n", r0, r1, r2);
  printf("__bss_start=%p - ", (void*)&__bss_start);
  printf("__bss_end=%p - ", (void*)&__bss_end);
  printf("__heap_start=%p - ", (void*)&__heap_start);
  printf("__stack_top=%p - ", (void*)&__stack_top);
  printf("__initcall_start=%p - ", (void*)&__initcall_start);
  printf("__initcall_end=%p\n", (void*)&__initcall_end);

  const char* cmdline = NULL;

  atag_header_t* curr = (atag_header_t*)r2;
  while (curr && curr->tag != 0) {
    switch (curr->tag) {
      case ATAG_CORE:
        printf("atag: core: %s\n", "???");
        break;
      case ATAG_MEM: {
        printf("atag: mem: start = %#010lx, size = %#010lx\n",
               ((atag_mem_t*)curr)->start,
               ((atag_mem_t*)curr)->size);
        break;
      }
      case ATAG_VIDEOTEXT:
        printf("atag: videotext: %s\n", "???");
        break;
      case ATAG_RAMDISK:
        printf("atag: ramdisk: %s\n", "???");
        break;
      case ATAG_INITRD2:
        printf("atag: initrd2: %s\n", "???");
        break;
      case ATAG_SERIAL:
        printf("atag: serial: %s\n", "???");
        break;
      case ATAG_REVISION:
        printf("atag: revision: %s\n", "???");
        break;
      case ATAG_VIDEOLFB:
        printf("atag: videolfb: %s\n", "???");
        break;
      case ATAG_CMDLINE: {
        atag_cmdline_t* ac = (atag_cmdline_t*)curr;
        cmdline = ac->cmdline;
        printf("atag: cmdline: %s\n", ac->cmdline);
        break;
      }
      default:
        printf("atag: unknown: tag=%lu size=%lu\n", curr->tag, curr->size);
    }

    curr = (atag_header_t*)((uint32_t*)curr + curr->size);
  }

  kmain_print_banner();

  print_step("initializing heap allocator");
  alloc_init();
  print_ok();

  print_step("initializing syscalls");
  syscall_init();
  print_ok();

  kmain_init_fs((uintptr_t)NULL);

  kmain_start(cmdline);
}
