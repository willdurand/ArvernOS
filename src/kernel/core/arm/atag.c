#include <core/arm/atag.h>

#include <arvern/utils.h>
#include <core/logging.h>
#include <stdint.h>

static const char* cmdline = NULL;
static uintptr_t initrd2_start_addr = 0;

void atag_init(atag_header_t* ptr)
{
  atag_header_t* curr = ptr;
  while (curr && curr->tag != ATAG_NONE) {
    switch (curr->tag) {
      case ATAG_CORE:
        CORE_DEBUG("atag: core: %s", "???");
        break;
      case ATAG_MEM: {
        CORE_DEBUG("atag: mem: start = %#010lx, size = %#010lx",
                   ((atag_mem_t*)curr)->start,
                   ((atag_mem_t*)curr)->size);
        break;
      }
      case ATAG_VIDEOTEXT:
        CORE_DEBUG("atag: videotext: %s", "???");
        break;
      case ATAG_RAMDISK:
        CORE_DEBUG("atag: ramdisk: %s", "???");
        break;
      case ATAG_INITRD2: {
        atag_initrd2_t* initrd2 = (atag_initrd2_t*)curr;
        initrd2_start_addr = initrd2->start;

        CORE_DEBUG("atag: initrd2: start=%p size=%#010lx",
                   initrd2_start_addr,
                   initrd2->size);
        break;
      }
      case ATAG_SERIAL:
        CORE_DEBUG("atag: serial: %s", "???");
        break;
      case ATAG_REVISION:
        CORE_DEBUG("atag: revision: %s", "???");
        break;
      case ATAG_VIDEOLFB:
        CORE_DEBUG("atag: videolfb: %s", "???");
        break;
      case ATAG_CMDLINE: {
        atag_cmdline_t* ac = (atag_cmdline_t*)curr;
        cmdline = ac->cmdline;
        CORE_DEBUG("atag: cmdline: %s", ac->cmdline);
        break;
      }
      default:
        CORE_DEBUG("atag: unknown: tag=%llu size=%llu", curr->tag, curr->size);
    }

    curr = (atag_header_t*)((uint32_t*)curr + curr->size);
  }
}

const char* atag_get_cmdline()
{
  return cmdline;
}

uintptr_t atag_get_initrd2_start_addr()
{
  return initrd2_start_addr;
}
