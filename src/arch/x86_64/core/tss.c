#include "tss.h"
#include <core/gdt.h>

// Defined in `src/arch/x86_64/boot.asm`.
extern tss_t tss;
extern gdt_table_t gdt64;

void tss_init()
{
  uint64_t tss_base = ((uint64_t)&tss);

  gdt64.tss_low.base15_0 = tss_base & 0xffff;
  gdt64.tss_low.base23_16 = (tss_base >> 16) & 0xff;
  gdt64.tss_low.base31_24 = (tss_base >> 24) & 0xff;
  gdt64.tss_low.limit15_0 = sizeof(tss_t);
  gdt64.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;
  gdt64.tss_high.base15_0 = (tss_base >> 48) & 0xffff;

  // Load the TSS.
  __asm__("ltr %%ax" : : "a"(TSS_SEGMENT));
}
