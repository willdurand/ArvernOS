CC   ?= gcc
LD   ?= ld
AR   ?= ar
NASM ?= nasm
QEMU ?= qemu-system-x86_64

ifeq ($(shell uname -s),Darwin)
	CC ?= x86_64-pc-elf-gcc
	LD ?= x86_64-pc-elf-ld
	AR ?= x86_64-pc-elf-ar
endif

OS_NAME    = willOS
BUILD_DIR  = build
LINKER     = linker.ld
ISO_DIR    = $(BUILD_DIR)/isofiles
KERNEL_DIR = $(ISO_DIR)/boot
GRUB_DIR   = $(KERNEL_DIR)/grub
KERNEL     = $(KERNEL_DIR)/kernel.bin
ISO        = $(BUILD_DIR)/$(OS_NAME).iso
LIBC       = $(BUILD_DIR)/libc-$(OS_NAME).a
LIBK       = $(BUILD_DIR)/libk-$(OS_NAME).a

OBJECTS := $(patsubst %.asm,%.o,$(shell find asm -name '*.asm'))
LIBK_SOURCES := $(patsubst %.c,%_k.o,$(shell find libs src -name '*.c'))
LIBC_SOURCES := $(patsubst %.c,%.o,$(shell find src/libc -name '*.c'))

CFLAGS = -W -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
				 -fno-builtin -fno-stack-protector -mno-red-zone \
				 -I src/include/ -I src/ -I libs/

DEBUG_CFLAGS = -DENABLE_KERNEL_DEBUG -DDEBUG_WITH_COLORS -DDISABLE_MMU_DEBUG

default: iso

kernel: ## compile the kernel
kernel: $(KERNEL)
.PHONY: kernel

$(KERNEL): $(OBJECTS) $(LIBK)
	mkdir -p $(KERNEL_DIR)
	$(LD) --nmagic --output=$@ --script=$(LINKER) $(OBJECTS) $(LIBK)

$(OBJECTS): %.o: %.asm
	mkdir -p $(BUILD_DIR)
	$(NASM) -f elf64 $<

$(LIBK_SOURCES): CFLAGS += -D__is_libk
$(LIBK_SOURCES): %_k.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC_SOURCES): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBK): $(LIBK_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

$(LIBC): $(LIBC_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

libc: ## build the libc (for userland)
libc: $(LIBC)
.PHONY: libc

iso: ## build the image of the OS (.iso)
iso: $(ISO)
.PHONY: iso

$(ISO): $(KERNEL) init
	mkdir -p $(GRUB_DIR)
	cp -R grub/* $(GRUB_DIR)
	cp init/init $(KERNEL_DIR)
	grub-mkrescue -o $@ $(ISO_DIR)

run: ## run the OS
run: $(ISO)
	$(QEMU) -cdrom $<
.PHONY: run

debug: ## build and run the OS in debug mode
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(ISO)
	$(QEMU) -cdrom $< -serial file:/tmp/serial.log
.PHONY: debug

clean: ## remove build artifacts
	find . -name '*.orig' -exec rm "{}" ";"
	rm -f $(OBJECTS) $(LIBK_SOURCES) $(LIBC_SOURCES) $(KERNEL) $(ISO) $(LIBK) $(LIBC)
	rm -rf $(BUILD_DIR)
	$(MAKE) -C init/ clean
.PHONY: clean

fmt: ## automatically format the code with astyle
	astyle --project=.astylerc --recursive "*.c,*.h"
	@find . -name '*.orig' -exec rm "{}" ";"
.PHONY: fmt

gdb: ## build, run the OS in debug mode and GDB
gdb: CFLAGS += $(DEBUG_CFLAGS) -g -O0
gdb: $(ISO)
	qemu-system-x86_64 -s -S -cdrom $< -serial file:/tmp/serial.log
.PHONY: gdb

init: ## compile the 'init' program (statically linked to libc)
init: libc
	rm -f init/init
	$(MAKE) -C init/
.PHONY: init

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
