NASM = nasm
QEMU = qemu-system-x86_64

ifeq ($(shell uname -s),Darwin)
	CC = x86_64-pc-elf-gcc
	LD = x86_64-pc-elf-ld
	AR = x86_64-pc-elf-ar
else
	CC = gcc
	LD = ld
	AR = ar
endif

OS_NAME    = willOS
BUILD_DIR  = build
LINKER     = linker.ld
ISO_DIR    = $(BUILD_DIR)/isofiles
KERNEL_DIR = $(ISO_DIR)/boot
GRUB_DIR   = $(KERNEL_DIR)/grub
GRUB_CFG   = $(GRUB_DIR)/grub.cfg
KERNEL_BIN = kernel.bin
KERNEL     = $(KERNEL_DIR)/$(KERNEL_BIN)
ISO        = $(BUILD_DIR)/$(OS_NAME).iso
LIBC       = $(BUILD_DIR)/libc-$(OS_NAME).a
LIBK       = $(BUILD_DIR)/libk-$(OS_NAME).a
INITRD_DIR = initrd
INITRD_TAR = initrd.tar
INITRD     = $(KERNEL_DIR)/$(INITRD_TAR)

ASM_OBJECTS  := $(patsubst %.asm,%.o,$(shell find asm -name '*.asm'))
LIBK_OBJECTS := $(patsubst %.c,%_k.o,$(shell find libs src -name '*.c'))
LIBC_OBJECTS := $(patsubst %.c,%.o,$(shell find src/libc libs -name '*.c'))
TEST_FILES 	 := $(patsubst test/%.c,%,$(shell find test/libc -name '*.c'))

GIT_HASH := $(shell git rev-parse --short HEAD)

CFLAGS = -DKERNEL_NAME=\"$(OS_NAME)\" \
				 -DGIT_HASH=\"$(GIT_HASH)\" \
				 -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
				 -fno-builtin -fstack-protector-all -mno-red-zone \
				 -I src/include/ -I src/ -I libs/

DEBUG_CFLAGS = -DENABLE_KERNEL_DEBUG -DDEBUG_WITH_COLORS -DDISABLE_MMU_DEBUG

default: iso

kernel: ## compile the kernel
kernel: $(KERNEL)
.PHONY: kernel

$(KERNEL): $(ASM_OBJECTS) $(LIBK)
	mkdir -p $(KERNEL_DIR)
	$(LD) --nmagic --output=$@ --script=$(LINKER) $(ASM_OBJECTS) $(LIBK)

$(ASM_OBJECTS): %.o: %.asm
	mkdir -p $(BUILD_DIR)
	$(NASM) -f elf64 $<

$(LIBK_OBJECTS): CFLAGS += -D__is_libk
$(LIBK_OBJECTS): %_k.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC_OBJECTS): CFLAGS += -D__is_libc
$(LIBC_OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBK): $(LIBK_OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

$(LIBC): $(LIBC_OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^

libc: ## build the libc (for userland)
libc: $(LIBC)
.PHONY: libc

iso: ## build the image of the OS (.iso)
iso: $(ISO)
.PHONY: iso

# See: https://stackoverflow.com/questions/649246/is-it-possible-to-create-a-multi-line-string-variable-in-a-makefile/649462#649462
define GRUB_CFG_BODY
set timeout=0
set default=0

menuentry "$(OS_NAME)" {
    multiboot2 /boot/$(KERNEL_BIN)
    module2 /boot/$(INITRD_TAR)
    boot
}
endef

export GRUB_CFG_BODY

$(GRUB_CFG):
	mkdir -p $(GRUB_DIR)
	echo "$$GRUB_CFG_BODY" > $@

$(ISO): $(KERNEL) $(INITRD) $(GRUB_CFG)
	grub-mkrescue -o $@ $(ISO_DIR)

run: ## run the OS
run: $(ISO)
	$(QEMU) -cdrom $<
.PHONY: run

debug: ## build and run the OS in debug mode
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(ISO)
	$(QEMU) -cdrom $< -serial file:/tmp/serial.log -m 1G
.PHONY: debug

clean: ## remove build artifacts
	find . -name '*.orig' -exec rm "{}" ";"
	find . -name '*.o' -exec rm "{}" ";"
	rm -rf $(BUILD_DIR) userland/bin/ $(INITRD_DIR)/{info,bin/,proc/}
.PHONY: clean

fmt: ## automatically format the code with clang-format
	find . -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -style=file -i "{}" ";"
.PHONY: fmt

gdb: ## build, run the OS in debug mode and GDB
gdb: CFLAGS += $(DEBUG_CFLAGS) -g -O0
gdb: $(ISO)
	qemu-system-x86_64 -s -S -cdrom $< -serial file:/tmp/serial.log
.PHONY: gdb

userland: ## compile the userland programs (statically linked to libc)
userland: libc
	$(MAKE) -C userland/init/ OS_NAME="$(OS_NAME)"
.PHONY: userland

test: ## run unit tests
test: CC=gcc
test: LD=ld
test: AR=ar
test: CFLAGS += -fPIC
test: libc
	# libc
	mkdir -p $(BUILD_DIR)/libc/string
	for file in $(TEST_FILES); do \
		echo ; \
		gcc -shared src/$$file.o -o build/$$file.so ; \
		gcc -I./test/ -O0 test/$$file.c -o build/$$file ; \
		LD_PRELOAD=./build/$$file.so ./build/$$file || exit 1 ; \
	done
	# tar
	gcc -g -DENABLE_DEBUG_FOR_TEST -I./test -I./src/ -o $(BUILD_DIR)/tar test/fs/tar.c src/fs/tar.c src/fs/vfs.c
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(BUILD_DIR)/tar
	# vfs
	gcc -DENABLE_DEBUG_FOR_TEST -I./test -I./src/ -o $(BUILD_DIR)/vfs test/fs/vfs.c src/fs/vfs.c
	./$(BUILD_DIR)/vfs
	# proc
	gcc -g -DENABLE_DEBUG_FOR_TEST -I./test -I./src/ -o $(BUILD_DIR)/proc test/fs/proc.c src/fs/proc.c src/fs/vfs.c
	valgrind --track-origins=yes --leak-check=yes ./$(BUILD_DIR)/proc
.PHONY: test

version: ## print tool versions
	$(CC) --version
	$(LD) --version
.PHONY: version

$(INITRD): userland
	cp -R userland/bin $(INITRD_DIR)
	echo "$(OS_NAME) build info\n\nhash: $(GIT_HASH)\ndate: $(shell date)" > $(INITRD_DIR)/info
	mkdir -p $(INITRD_DIR)/proc
	cd $(INITRD_DIR) && tar -cvf ../$(INITRD) *

initrd: ## build the init ram disk
initrd: $(INITRD)
.PHONY: initrd

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
