CC   ?= gcc
LD   ?= ld
AR   ?= ar
NASM ?= nasm

ifeq ($(shell uname -s),Darwin)
	CC = x86_64-pc-elf-gcc
	LD = x86_64-pc-elf-ld
	AR = x86_64-pc-elf-ar
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
SOURCES := $(patsubst %.c,%.o,$(shell find src -name '*.c'))
LIBC_SOURCES := $(patsubst %.c,%.o,$(shell find src/libc -name '*.c'))

CFLAGS = -W -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
		 -fno-builtin -fno-stack-protector \
		 -mno-red-zone \
		 -I src/include/ -I src/ \
		 -D__is_libk

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

$(SOURCES): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBK): $(SOURCES)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

$(LIBC): $(LIBC_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

libc: ## build the libc (for userland)
libc: CFLAGS := $(filter-out -D__is_libk, $(CFLAGS))
libc: $(LIBC)
.PHONY: libc

iso: ## build the image of the OS (.iso)
iso: $(ISO)
.PHONY: iso

$(ISO): $(KERNEL)
	mkdir -p $(GRUB_DIR)
	cp -R grub/* $(GRUB_DIR)
	grub-mkrescue -o $@ $(ISO_DIR)

run: ## run the OS
run: $(ISO)
	qemu-system-x86_64 -cdrom $<
.PHONY: run

debug: ## build and run the OS in debug mode
debug: CFLAGS += -DENABLE_KERNEL_DEBUG
debug: $(ISO)
	qemu-system-x86_64 -cdrom $< -serial file:/tmp/serial.log
.PHONY: debug

clean: ## remove build artifacts
	find . -name '*.orig' -exec rm "{}" ";"
	rm -f $(OBJECTS) $(SOURCES) $(KERNEL) $(ISO) $(LIBK) $(LIBC)
	rm -rf $(BUILD_DIR)
.PHONY: clean

fmt: ## automatically format the code with astyle
	astyle --project=.astylerc --recursive "*.c,*.h"
	@find . -name '*.orig' -exec rm "{}" ";"
.PHONY: fmt

gdb: ## build, run the OS in debug mode and GDB
gdb: CFLAGS += -DENABLE_KERNEL_DEBUG -g
gdb: $(ISO)
	qemu-system-x86_64 -s -S -cdrom $< -serial file:/tmp/serial.log
.PHONY: gdb

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
