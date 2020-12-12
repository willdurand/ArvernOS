NASM = nasm
QEMU = qemu-system-x86_64

ifeq ($(shell uname -s),Darwin)
	CC = x86_64-elf-gcc
	LD = x86_64-elf-ld
	AR = x86_64-elf-ar
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
TEST_FILES   := $(patsubst test/%.c,%,$(shell find test/libc -name '*.c'))

GIT_HASH := $(shell git rev-parse --short HEAD)

CFLAGS = -DKERNEL_NAME=\"$(OS_NAME)\" \
	 -DGIT_HASH=\"$(GIT_HASH)\" \
	 -Wall -pedantic -std=c11 -O0 -ffreestanding -nostdlib \
	 -fno-builtin -fstack-protector -mno-red-zone \
	 -I src/ -I src/include/ -I libs/

DEBUG_CFLAGS = -DENABLE_KERNEL_DEBUG -DDEBUG_WITH_COLORS -DDISABLE_MMU_DEBUG

QEMU_OPTIONS = -m 500M \
	       -netdev user,id=u1,ipv6=off \
	       -device rtl8139,netdev=u1 \
	       -object filter-dump,id=f1,netdev=u1,file=./logs/traffic.pcap

GRUB_KERNEL_CMDLINE =

# See: https://stackoverflow.com/questions/649246/is-it-possible-to-create-a-multi-line-string-variable-in-a-makefile/649462#649462
define GRUB_CFG_BODY
set timeout=0
set default=0

menuentry "$(OS_NAME)" {
    multiboot2 /boot/$(KERNEL_BIN) $(GRUB_KERNEL_CMDLINE)
    module2 /boot/$(INITRD_TAR)
    boot
}
endef

export GRUB_CFG_BODY

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

$(GRUB_CFG):
	mkdir -p $(GRUB_DIR)
	echo "$$GRUB_CFG_BODY" > $@

$(ISO): $(KERNEL) $(INITRD) $(GRUB_CFG)
	grub-mkrescue -o $@ $(ISO_DIR)

run: ## run the OS
run: $(ISO)
	$(QEMU) -cdrom $< $(QEMU_OPTIONS)
.PHONY: run

debug: ## build the OS in debug mode
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(ISO)
.PHONY: debug

run-debug: ## run the OS in debug mode
run-debug: QEMU_OPTIONS += -serial file:./logs/debug.log -d int --no-reboot
run-debug: debug run
.PHONY: run-debug

run-test: ## run the OS in test mode
run-test: QEMU_OPTIONS += -curses
run-test: GRUB_KERNEL_CMDLINE = "boot-and-exit"
run-test: run
.PHONY: run-test

clean: ## remove build artifacts
	find . -name '*.orig' -exec rm "{}" ";"
	find . -name '*.o' -exec rm "{}" ";"
	rm -rf $(BUILD_DIR) userland/bin/ $(INITRD_DIR)/{info,bin/,proc/}
.PHONY: clean

fmt: ## automatically format the code with clang-format
	find . -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -style=file -i "{}" ";"
.PHONY: fmt

gdb: ## build, run the OS in debug mode and enable GDB
gdb: CFLAGS += -g -O0
gdb: QEMU_OPTIONS += -s -S
gdb: debug run
.PHONY: gdb

userland: ## compile the userland programs (statically linked to libc)
userland: libc
	@for userland_program in $(shell find userland/* -type d -not \( -path userland/bin -o -path userland/local-build \)); do \
	    $(MAKE) -C $$userland_program OS_NAME="$(OS_NAME)" ; \
	done
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
	# fs/tar
	gcc -DENABLE_DEBUG_FOR_TEST -I./test/ -I./src/ -o $(BUILD_DIR)/tar test/fs/tar.c src/fs/tar.c src/fs/vfs.c
	./$(BUILD_DIR)/tar
	# fs/vfs
	gcc -DENABLE_DEBUG_FOR_TEST -I./test/ -I./src/ -o $(BUILD_DIR)/vfs test/fs/vfs.c src/fs/vfs.c
	./$(BUILD_DIR)/vfs
	# fs/proc
	gcc -g -DENABLE_DEBUG_FOR_TEST -I./test/ -I./test/proxies/ -I./src/ -o $(BUILD_DIR)/proc test/fs/proc.c src/fs/proc.c src/fs/vfs.c
	# mmu/frame
	gcc -Wformat=0 -g -DENABLE_DEBUG_FOR_TEST -I./test/ -I./test/proxies/ -I./src/ -o $(BUILD_DIR)/frame test/mmu/frame.c src/mmu/frame.c src/core/multiboot.c src/mmu/bitmap.c
	valgrind --track-origins=yes --leak-check=yes ./$(BUILD_DIR)/frame
	# mmu/bitmap
	gcc -g -DENABLE_DEBUG_FOR_TEST -I./test/ -I./src/ -o $(BUILD_DIR)/bitmap test/mmu/bitmap.c src/mmu/bitmap.c
	./$(BUILD_DIR)/bitmap
	# mmu/paging
	gcc -Wformat=0 -g -DENABLE_DEBUG_FOR_TEST -DTEST_ENV -I./test/ -I./test/proxies/ -I./src/ -o $(BUILD_DIR)/paging test/mmu/paging.c src/mmu/paging.c src/core/multiboot.c src/mmu/frame.c src/mmu/bitmap.c src/core/register.c
	./$(BUILD_DIR)/paging
	# config/inish
	gcc -DENABLE_DEBUG_FOR_TEST -I./test/ -I./test/proxies/ -I./src/ -o $(BUILD_DIR)/inish test/config/inish.c src/config/inish.c
	valgrind --track-origins=yes --leak-check=yes ./$(BUILD_DIR)/inish
.PHONY: test

version: ## print tool versions
	$(CC) --version
	$(LD) --version
	$(QEMU) --version
.PHONY: version

$(INITRD): userland
	cp -R userland/bin $(INITRD_DIR)
	echo "$(OS_NAME) build info\n\nhash: $(GIT_HASH)\ndate: $(shell date)" > $(INITRD_DIR)/info
	mkdir -p $(INITRD_DIR)/proc
	cd $(INITRD_DIR) && tar -cvf ../$(INITRD) *

initrd: ## build the init ram disk
initrd: $(INITRD)
.PHONY: initrd

docs: ## build the docs
	rm -rf docs/*
	doxygen ./Doxyfile
.PHONY: docs

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
