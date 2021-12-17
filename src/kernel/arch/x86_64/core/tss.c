#include "tss.h"
#include <core/gdt.h>
#include <core/logging.h>

// Defined in `src/arch/x86_64/boot.asm`.
extern tss_t tss;
extern gdt_table_t gdt64;

void tss_init()
{
  INFO("core: initialize TSS - rsp0=0x%02x rsp1=0x%02x rsp2=0x%02x",
       tss.rsp0,
       tss.rsp1,
       tss.rsp2);

  uint64_t tss_base = ((uint64_t)&tss);

  gdt64.tss_low.base15_0 = tss_base & 0xffff;
  gdt64.tss_low.base23_16 = (tss_base >> 16) & 0xff;
  gdt64.tss_low.base31_24 = (tss_base >> 24) & 0xff;
  gdt64.tss_low.limit15_0 = sizeof(tss_t);
  gdt64.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;
  gdt64.tss_high.base15_0 = (tss_base >> 48) & 0xffff;

  // Load the TSS.
  __asm__("ltr %%ax" : : "a"(TSS_SEGMENT));

  DEBUG("gdt64.kernel_code: type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.kernel_code.type,
        gdt64.kernel_code.limit19_16_and_flags);
  DEBUG("gdt64.kernel_data: type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.kernel_data.type,
        gdt64.kernel_data.limit19_16_and_flags);
  DEBUG("gdt64.user_code  : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.user_code.type,
        gdt64.user_code.limit19_16_and_flags);
  DEBUG("gdt64.user_data  : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.user_data.type,
        gdt64.user_data.limit19_16_and_flags);
  DEBUG("gdt64.tss_low    : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.tss_low.type,
        gdt64.tss_low.limit19_16_and_flags);
  DEBUG("gdt64.tss_high   : type=0x%02x limit19_16_and_flags=0x%02x",
        gdt64.tss_high.type,
        gdt64.tss_high.limit19_16_and_flags);
}
