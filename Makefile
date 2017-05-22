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
LIB        = $(BUILD_DIR)/lib$(OS_NAME).a

OBJECTS := $(patsubst %.asm,%.o,$(shell find asm -name '*.asm'))
SOURCES := $(patsubst %.c,%.o,$(shell find src -name '*.c'))

CFLAGS = -W -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
		 -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -mno-red-zone \
		 -I src/include/ -I src/

default: iso

kernel: $(KERNEL)
.PHONY: kernel

$(KERNEL): $(OBJECTS) $(LIB)
	mkdir -p $(KERNEL_DIR)
	$(LD) --nmagic --output=$@ --script=$(LINKER) $(OBJECTS) $(LIB)

$(OBJECTS): %.o: %.asm
	mkdir -p $(BUILD_DIR)
	$(NASM) -f elf64 $<

$(SOURCES): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(SOURCES)
	$(AR) rcs $@ $^

iso: $(ISO)
.PHONY: iso

$(ISO): $(KERNEL)
	mkdir -p $(GRUB_DIR)
	cp -R grub/* $(GRUB_DIR)
	grub-mkrescue -o $@ $(ISO_DIR)

run: $(ISO)
	qemu-system-x86_64 -cdrom $<
.PHONY: run

run-i386: $(ISO)
	qemu-system-i386 -cdrom $<
.PHONY: run-i386

debug: CFLAGS += -DENABLE_KERNEL_DEBUG
debug: $(ISO)
	qemu-system-x86_64 -cdrom $< -serial file:/tmp/serial.log
.PHONY: run

clean:
	rm -f $(OBJECTS) $(SOURCES) $(KERNEL) $(ISO) $(LIB)
	rm -rf $(BUILD_DIR)
.PHONY: clean
