#ifndef CORE_ARM_ATAG_H
#define CORE_ARM_ATAG_H

#include <stdint.h>

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

void atag_init(atag_header_t* ptr);

const char* atag_get_cmdline();

#endif
