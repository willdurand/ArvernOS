#ifndef CORE_GDT_H
#define CORE_GDT_H

#define KERNEL_BASE_SELECTOR 0x08

typedef struct gdt_descriptor
{
  uint16_t limit15_0;
  uint16_t base15_0;
  uint8_t base23_16;
  uint8_t type;
  uint8_t limit19_16_and_flags;
  uint8_t base31_24;
} gdt_descriptor_t;

typedef struct gdt_table
{
  gdt_descriptor_t null_1;
  gdt_descriptor_t kernel_code;
  gdt_descriptor_t kernel_data;
  gdt_descriptor_t tss_low;
  gdt_descriptor_t tss_high;
} gdt_table_t;

#endif
