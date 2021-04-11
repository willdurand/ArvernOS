ARCH ?= x86_64

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

OS_NAME        = willOS
BUILD_DIR      = build
ARCH_BUILD_DIR := $(BUILD_DIR)/$(ARCH)
DIST_DIR       := $(ARCH_BUILD_DIR)/dist
ISO_DIR        := $(ARCH_BUILD_DIR)/isofiles
ISO_BOOT_DIR   := $(ISO_DIR)/boot
GRUB_DIR       := $(ISO_BOOT_DIR)/grub
GRUB_CFG       := $(GRUB_DIR)/grub.cfg
ARCH_SRC       := src/arch/$(ARCH)
LINKER         := $(ARCH_SRC)/linker.ld
KERNEL_BIN     := kernel-$(ARCH).bin
KERNEL         := $(ISO_BOOT_DIR)/$(KERNEL_BIN)
ISO            := $(DIST_DIR)/$(OS_NAME)-$(ARCH).iso
LIBC           := $(DIST_DIR)/libc-$(OS_NAME)-$(ARCH).a
LIBC_OBJS_DIR  := $(ARCH_BUILD_DIR)/libc-objects
LIBK_OBJS_DIR  := $(ARCH_BUILD_DIR)/libk-objects
INITRD_DIR     = initrd
INITRD_TAR     = initrd.tar
INITRD         := $(ISO_BOOT_DIR)/$(INITRD_TAR)
EXTERNAL_DIR   = external
GIT_HASH       := $(shell git rev-parse --short HEAD)
BUILD_MODE     = release
# This should be a bitmap font.
KERNEL_CONSOLE_FONT_PATH = external/scalable-font2/fonts/u_vga16.sfn.gz
KERNEL_CONSOLE_FONT      := $(ARCH_BUILD_DIR)/font.o
# This is used in `src/asm/multiboot_header.asm`.
VBE_WIDTH  = 1024
VBE_HEIGHT = 768
VBE_BPP    = 32

# This is the list of external libraries we use and need to build for the
# kernel (libk) and the libc.
EXTERNAL_DEPS = liballoc printf vtconsole

EXTERNAL_DIRS    := $(addprefix $(EXTERNAL_DIR)/,$(EXTERNAL_DEPS))
LIBK_OBJECTS     := $(patsubst %.c, $(LIBK_OBJS_DIR)/%.o, $(shell find $(ARCH_SRC) src/libc src/graphics $(EXTERNAL_DIRS) -name '*.c'))
LIBK_ASM_OBJECTS := $(patsubst %.asm, $(LIBK_OBJS_DIR)/%.o, $(shell find $(ARCH_SRC)/asm -name '*.asm'))
LIBC_OBJECTS     := $(patsubst %.c, $(LIBC_OBJS_DIR)/%.o, $(shell find src/libc $(EXTERNAL_DIRS) -name '*.c'))
LIBC_ASM_OBJECTS := $(patsubst %.asm, $(LIBC_OBJS_DIR)/%.o, $(shell find src/libc/asm -name '*.asm'))
LIBC_TEST_FILES  := $(patsubst test/%.c, %, $(shell find test/libc -name '*.c'))

NASM_OPTIONS := -dVBE_WIDTH=$(VBE_WIDTH) -dVBE_HEIGHT=$(VBE_HEIGHT) -dVBE_BPP=$(VBE_BPP)

QEMU_OPTIONS = -m 500M \
	       -netdev user,id=u1,ipv6=off,dhcpstart=10.0.2.20 \
	       -device rtl8139,netdev=u1 \
	       -object filter-dump,id=f1,netdev=u1,file=./log/traffic.pcap

 # We need to have -fno-omit-frame-pointer or the kernel stack backtrace won't get the stack
 CFLAGS := -DKERNEL_NAME=\"$(OS_NAME)\" \
	 -DGIT_HASH=\"$(GIT_HASH)\" \
	 -DLOGS_WITH_COLORS \
	 -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
	 -fno-builtin -fstack-protector -mno-red-zone -fno-omit-frame-pointer \
	 -I src/ -I src/arch/$(ARCH)/ -I include/ $(addprefix -I$(EXTERNAL_DIR)/,$(EXTERNAL_DEPS)) \
	 -I $(EXTERNAL_DIR)/scalable-font2/

DEBUG_CFLAGS = -g -DENABLE_KERNEL_DEBUG

ifeq ($(ENABLE_CONFIG_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_CONFIG_DEBUG
endif

ifeq ($(ENABLE_CORE_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_CORE_DEBUG
endif

ifeq ($(ENABLE_FS_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_FS_DEBUG
endif

ifeq ($(ENABLE_MMU_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_MMU_DEBUG
endif

ifeq ($(ENABLE_NET_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_NET_DEBUG
endif

ifeq ($(ENABLE_PROC_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_PROC_DEBUG
endif

ifeq ($(ENABLE_SYS_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_SYS_DEBUG
endif

ifeq ($(ENABLE_USERLAND_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_USERLAND_DEBUG
endif

ifeq ($(ENABLE_ALL_DEBUG), 1)
	DEBUG_CFLAGS += -DENABLE_CONFIG_DEBUG -DENABLE_CORE_DEBUG -DENABLE_FS_DEBUG -DENABLE_MMU_DEBUG -DENABLE_NET_DEBUG -DENABLE_PROC_DEBUG -DENABLE_SYS_DEBUG -DENABLE_USERLAND_DEBUG
endif

ifeq ($(ENABLE_FRAMEBUFFER), 1)
	CFLAGS += -DENABLE_FRAMEBUFFER
	NASM_OPTIONS += -dENABLE_FRAMEBUFFER
endif

ifeq ($(ENABLE_FRAMEBUFFER_STATS), 1)
	CFLAGS += -DENABLE_FRAMEBUFFER_STATS
endif

ifeq ($(ENABLE_SSE2), 1)
	NASM_OPTIONS += -dENABLE_SSE2
else
	CFLAGS += -mno-sse2
endif

GRUB_KERNEL_CMDLINE ?= /bin/init -s
GRUB_DEFAULT ?= 0

# See: https://stackoverflow.com/questions/649246/is-it-possible-to-create-a-multi-line-string-variable-in-a-makefile/649462#649462
define GRUB_CFG_BODY
set timeout=1
set default=$(GRUB_DEFAULT)

menuentry "$(OS_NAME) $(BUILD_MODE)" {
	multiboot2 /boot/$(KERNEL_BIN) $(GRUB_KERNEL_CMDLINE)
	module2 /boot/$(INITRD_TAR)
	boot
}

menuentry "$(OS_NAME) $(BUILD_MODE) (kernel mode)" {
	multiboot2 /boot/$(KERNEL_BIN) kshell
	module2 /boot/$(INITRD_TAR)
	boot
}
endef

export GRUB_CFG_BODY

default: help

$(BUILD_DIR):
	mkdir -p $@

$(ARCH_BUILD_DIR): $(BUILD_DIR)
	mkdir -p $@

$(DIST_DIR): $(ARCH_BUILD_DIR)
	mkdir -p $@

$(ISO_DIR): $(ARCH_BUILD_DIR)
	mkdir -p $@

$(ISO_BOOT_DIR): $(ISO_DIR)
	mkdir -p $@

$(GRUB_DIR): $(ISO_BOOT_DIR)
	mkdir -p $@

$(KERNEL): $(ISO_BOOT_DIR) $(DIST_DIR) $(LIBK_ASM_OBJECTS) $(LIBK_OBJECTS) $(KERNEL_CONSOLE_FONT)
	$(LD) --nmagic --output=$@ --script=$(LINKER) $(LIBK_ASM_OBJECTS) $(LIBK_OBJECTS) $(KERNEL_CONSOLE_FONT)
	cp $(KERNEL) $(DIST_DIR)

kernel: ## compile the kernel
kernel: $(KERNEL)
.PHONY: kernel

$(LIBK_ASM_OBJECTS): $(LIBK_OBJS_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(NASM) $(NASM_OPTIONS) -f elf64 $< -o $@

$(LIBK_OBJECTS): CFLAGS += -D__is_libk
$(LIBK_OBJECTS): $(LIBK_OBJS_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC_ASM_OBJECTS): $(LIBC_OBJS_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(NASM) $(NASM_OPTIONS) -f elf64 $< -o $@

$(LIBC_OBJECTS): CFLAGS += -D__is_libc
$(LIBC_OBJECTS): $(LIBC_OBJS_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBC): $(DIST_DIR) $(LIBC_ASM_OBJECTS) $(LIBC_OBJECTS)
	$(AR) rcs $@ $(LIBC_ASM_OBJECTS) $(LIBC_OBJECTS)

libc: ## build the libc (userland)
libc: $(LIBC)
.PHONY: libc

$(KERNEL_CONSOLE_FONT): $(ARCH_BUILD_DIR)
	cp $(KERNEL_CONSOLE_FONT_PATH) $(ARCH_BUILD_DIR)/console.sfn.gz
	gunzip -f $(ARCH_BUILD_DIR)/console.sfn.gz
	# We have to do this because we cannot control the symbol name generated by
	# `ld`. We get a pretty nice name by cd'ing into the build directory.
	cd $(ARCH_BUILD_DIR) && $(LD) -r -b binary -o ../../$@ console.sfn

console-font: ## compile the (default) kernel console font
console-font: $(KERNEL_CONSOLE_FONT)
.PHONY: console-font

$(INITRD): $(INITRD_DIR) userland
	cp -R userland/bin $(INITRD_DIR)
	echo "$(OS_NAME) ($(ARCH)) build info\n\nhash: $(GIT_HASH)\ndate: $(shell date)" > $(INITRD_DIR)/info
	cd $(INITRD_DIR) && tar -cvf ../$(INITRD) *

initrd: ## build the init ram disk
initrd: $(INITRD)
.PHONY: initrd

# We mark this target as .PHONY to write the file every time.
$(GRUB_CFG): $(GRUB_DIR)
	echo "$$GRUB_CFG_BODY" > $@
.PHONY: $(GRUB_CFG)

$(ISO): $(DIST_DIR) $(KERNEL) $(INITRD) $(GRUB_CFG)
	grub-mkrescue -o $@ $(ISO_DIR)

iso: ## build the image of the OS (.iso)
iso: $(ISO)
.PHONY: iso

run: ## run the OS in release mode
run: QEMU_OPTIONS += -serial file:./log/release.log
run: $(ISO)
	$(QEMU) -cdrom $< $(QEMU_OPTIONS)
.PHONY: run

debug: ## build the OS in debug mode
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: BUILD_MODE = debug
debug: GRUB_DEFAULT = 1
debug: $(ISO)
.PHONY: debug

run-debug: ## run the OS in debug mode
run-debug: QEMU_OPTIONS += -serial file:./log/debug.log -d int --no-reboot
run-debug: debug
	$(QEMU) -cdrom $(ISO) $(QEMU_OPTIONS)
.PHONY: run-debug

run-test: ## run the OS in test mode
run-test: QEMU_OPTIONS += -curses -serial file:./log/test.log
run-test: GRUB_KERNEL_CMDLINE = /bin/boot-and-exit
run-test: run
.PHONY: run-test

clean: ## remove build artifacts
	rm -rf $(ARCH_BUILD_DIR) $(INITRD_DIR)/info $(INITRD_DIR)/bin/ userland/bin/ userland/local-build/
.PHONY: clean

fmt: ## automatically format the code with clang-format
	find . -path ./external -prune -false -o -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -style=file -i "{}" ";"
.PHONY: fmt

gdb: ## build, run the OS in debug mode and enable GDB
gdb: QEMU_OPTIONS += -nographic -s -S -serial file:./log/debug.log
gdb: debug run
.PHONY: gdb

userland: ## compile the userland programs (statically linked to libc)
userland: libc
	@for userland_program in $(shell find userland/* -type d -not \( -path userland/bin -o -path userland/local-build \)); do \
	    $(MAKE) -C $$userland_program OS_NAME="$(OS_NAME)" ARCH="$(ARCH)" ENABLE_USERLAND_DEBUG=$(ENABLE_USERLAND_DEBUG) ; \
	done
.PHONY: userland

test: ## run unit tests
test: CC=gcc
test: LD=ld
test: AR=ar
test: CFLAGS += -fPIC
test: CFLAGS_FOR_TESTS = -g -DENABLE_LOGS_FOR_TESTS -DTEST_ENV -I./test/ -I./src/ -I./src/arch/$(ARCH)/
test: VALGRIND_OPTS = --track-origins=yes --leak-check=yes
test: libc
	# libc
	mkdir -p $(ARCH_BUILD_DIR)/libc/string
	for file in $(LIBC_TEST_FILES); do \
		echo ; \
		gcc -shared $(LIBC_OBJS_DIR)/src/$$file.o -o $(ARCH_BUILD_DIR)/$$file.so ; \
		gcc -I./test/ test/$$file.c -o $(ARCH_BUILD_DIR)/$$file ; \
		LD_PRELOAD=./$(ARCH_BUILD_DIR)/$$file.so ./$(ARCH_BUILD_DIR)/$$file || exit 1 ; \
	done
	# fs/vfs
	gcc $(CFLAGS_FOR_TESTS) -I./test/proxies/ -o $(ARCH_BUILD_DIR)/vfs test/fs/vfs.c src/arch/$(ARCH)/fs/vfs.c
	valgrind --track-origins=yes --leak-check=yes ./$(ARCH_BUILD_DIR)/vfs
	# fs/tar
	gcc $(CFLAGS_FOR_TESTS) -o $(ARCH_BUILD_DIR)/tar test/fs/tar.c src/arch/$(ARCH)/fs/tar.c src/arch/$(ARCH)/fs/vfs.c
	valgrind $(VALGRIND_OPTS) ./$(ARCH_BUILD_DIR)/tar
	# fs/proc
	gcc $(CFLAGS_FOR_TESTS) -I./test/proxies/ -o $(ARCH_BUILD_DIR)/proc test/fs/proc.c src/arch/$(ARCH)/fs/proc.c src/arch/$(ARCH)/fs/vfs.c
	valgrind $(VALGRIND_OPTS) ./$(ARCH_BUILD_DIR)/proc
	# fs/sock
	gcc $(CFLAGS_FOR_TESTS) -o $(ARCH_BUILD_DIR)/sock test/fs/sock.c src/arch/$(ARCH)/fs/sock.c src/arch/$(ARCH)/fs/vfs.c
	valgrind $(VALGRIND_OPTS) ./$(ARCH_BUILD_DIR)/sock
	# mmu/frame
	gcc $(CFLAGS_FOR_TESTS) -Wformat=0 -I./test/proxies/ -o $(ARCH_BUILD_DIR)/frame test/mmu/frame.c src/arch/$(ARCH)/mmu/frame.c src/arch/$(ARCH)/core/multiboot.c src/arch/$(ARCH)/mmu/bitmap.c
	valgrind $(VALGRIND_OPTS) ./$(ARCH_BUILD_DIR)/frame
	# mmu/bitmap
	gcc $(CFLAGS_FOR_TESTS) -o $(ARCH_BUILD_DIR)/bitmap test/mmu/bitmap.c src/arch/$(ARCH)/mmu/bitmap.c
	./$(ARCH_BUILD_DIR)/bitmap
	# mmu/paging
	gcc $(CFLAGS_FOR_TESTS) -Wformat=0 -I./test/proxies/ -o $(ARCH_BUILD_DIR)/paging test/mmu/paging.c src/arch/$(ARCH)/mmu/paging.c src/arch/$(ARCH)/core/multiboot.c src/arch/$(ARCH)/mmu/frame.c src/arch/$(ARCH)/mmu/bitmap.c src/arch/$(ARCH)/core/register.c
	./$(ARCH_BUILD_DIR)/paging
	# config/inish
	gcc $(CFLAGS_FOR_TESTS) -I./test/proxies/ -o $(ARCH_BUILD_DIR)/inish test/config/inish.c src/arch/$(ARCH)/config/inish.c
	valgrind $(VALGRIND_OPTS) ./$(ARCH_BUILD_DIR)/inish
.PHONY: test

version: ## print tool versions
	$(CC) --version
	$(LD) --version
	$(AR) --version
	$(NASM) --version
	$(QEMU) --version
.PHONY: version

docs: ## build the docs
	rm -rf docs/*
	doxygen ./Doxyfile
.PHONY: docs

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' Makefile* | awk '{line = gensub(/^Makefile(-(.+)\.include)?:/, "ARCH=\\2 ", "g", $$0); gsub(/^ARCH= /, "", line); print line}' | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
