#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

typedef struct multiboot_info {
	uint32_t flags;
	uint32_t low_mem;
	uint32_t high_mem;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	struct {
		uint32_t num;
		uint32_t size;
		uint32_t addr;
		uint32_t shndx;
	} elf_sec;
	unsigned long mmap_length;
	unsigned long mmap_addr;
	unsigned long drives_length;
	unsigned long drives_addr;
	unsigned long config_table;
	unsigned long boot_loader_name;
	unsigned long apm_table;
	unsigned long vbe_control_info;
	unsigned long vbe_mode_info;
	unsigned long vbe_mode;
	unsigned long vbe_interface_seg;
	unsigned long vbe_interface_off;
	unsigned long vbe_interface_len;
} multiboot_info_t;

#endif
