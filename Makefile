include ./Makefile.include

ARCH           ?= x86_64
OS_NAME        = willOS
BUILD_MODE     = release
# We (more or less) follow the PFL project structure:
# https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#intro.dirs
BUILD_DIR      = build
EXTERNAL_DIR   = external
TOOLS_DIR      = tools
LOG_DIR        = log
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
GIT_HASH       := $(shell git rev-parse --short HEAD)
SYMBOLS_TXT    := symbols.txt
SYMBOLS        := $(ISO_BOOT_DIR)/$(SYMBOLS_TXT)
LOG_FILE       = $(LOG_DIR)/$(BUILD_MODE).log

# This should be a bitmap font.
KERNEL_CONSOLE_FONT_PATH := $(EXTERNAL_DIR)/scalable-font2/fonts/u_vga16.sfn.gz
KERNEL_CONSOLE_FONT      := $(ARCH_BUILD_DIR)/font.o
# This is used in `src/arch/x86_64/asm/multiboot_header.asm`.
VBE_WIDTH  = 1024
VBE_HEIGHT = 768
VBE_BPP    = 32

# More tools.
NASM = nasm
QEMU = qemu-system-x86_64

# This is the list of external libraries we use and need to build for the
# kernel (libk) and the libc.
EXTERNAL_DEPS = liballoc printf vtconsole

EXTERNAL_DIRS    := $(addprefix $(EXTERNAL_DIR)/,$(EXTERNAL_DEPS))
LIBK_OBJECTS     := $(patsubst %.c, $(LIBK_OBJS_DIR)/%.o, $(shell find $(ARCH_SRC) src/libc $(EXTERNAL_DIRS) -name '*.c'))
LIBK_ASM_OBJECTS := $(patsubst %.asm, $(LIBK_OBJS_DIR)/%.o, $(shell find $(ARCH_SRC)/asm -name '*.asm'))
LIBC_OBJECTS     := $(patsubst %.c, $(LIBC_OBJS_DIR)/%.o, $(shell find src/libc $(EXTERNAL_DIRS) -name '*.c'))
LIBC_ASM_OBJECTS := $(patsubst %.asm, $(LIBC_OBJS_DIR)/%.o, $(shell find src/libc/asm -name '*.asm'))
LIBC_TEST_FILES  := $(patsubst test/%.c, %, $(shell find test/libc -name '*.c'))

NASM_OPTIONS := -dVBE_WIDTH=$(VBE_WIDTH) -dVBE_HEIGHT=$(VBE_HEIGHT) -dVBE_BPP=$(VBE_BPP)

QEMU_OPTIONS += -serial file:$(LOG_FILE)
QEMU_OPTIONS += -m 512M
QEMU_OPTIONS += -netdev user,id=u1,ipv6=off,dhcpstart=10.0.2.20
QEMU_OPTIONS += -device rtl8139,netdev=u1
QEMU_OPTIONS += -object filter-dump,id=f1,netdev=u1,file=$(LOG_DIR)/traffic-$(BUILD_MODE).pcap

INCLUDES += -Isrc/
INCLUDES += -Isrc/arch/$(ARCH)/
INCLUDES += -Iinclude/
INCLUDES += $(addprefix -I$(EXTERNAL_DIR)/,$(EXTERNAL_DEPS))
INCLUDES += -I$(EXTERNAL_DIR)/scalable-font2/

WERRORS += -Wall -Wextra -Werror
WERRORS += -Werror=implicit-function-declaration
WERRORS += -Werror=int-conversion
WERRORS += -Werror=incompatible-pointer-types
WERRORS += -Werror=shift-count-overflow
WERRORS += -Werror=switch

CFLAGS += -DKERNEL_NAME=\"$(OS_NAME)\"
CFLAGS += -DGIT_HASH=\"$(GIT_HASH)\"
CFLAGS += -DLOGS_WITH_COLORS
CFLAGS += -O2 -std=c11 -ffreestanding -nostdinc -nostdlib -fno-builtin
# We need to have -fno-omit-frame-pointer or the kernel stack backtrace won't
# get the stack.
CFLAGS += --target=x86_64 -fno-omit-frame-pointer -fstack-protector-strong
CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-avx -mno-avx2
CFLAGS += $(WERRORS)

DEBUG_CFLAGS = -g3 -DENABLE_KERNEL_DEBUG

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

GRUB_KERNEL_CMDLINE ?= /bin/init -s
GRUB_DEFAULT ?= 0

# See: https://stackoverflow.com/questions/649246/is-it-possible-to-create-a-multi-line-string-variable-in-a-makefile/649462#649462
define GRUB_CFG_BODY
set timeout=1
set default=$(GRUB_DEFAULT)

menuentry "$(OS_NAME) $(BUILD_MODE)" {
	multiboot2 /boot/$(KERNEL_BIN) $(GRUB_KERNEL_CMDLINE)
	module2 /boot/$(INITRD_TAR) initrd
	module2 /boot/$(SYMBOLS_TXT) symbols
	boot
}

menuentry "$(OS_NAME) $(BUILD_MODE) (kernel mode)" {
	multiboot2 /boot/$(KERNEL_BIN) kshell
	module2 /boot/$(INITRD_TAR) initrd
	module2 /boot/$(SYMBOLS_TXT) symbols
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
	$(PROGRESS) "LD" $@
	$(LD) --nmagic --output=$@ --script=$(LINKER) $(LIBK_ASM_OBJECTS) $(LIBK_OBJECTS) $(KERNEL_CONSOLE_FONT)
	$(NM) $@ | awk '{ print $$1, $$3 }' | sort > $(SYMBOLS)
	cp $@ $(DIST_DIR)

kernel: ## compile the kernel
kernel: $(KERNEL)
.PHONY: kernel

$(LIBK_ASM_OBJECTS): $(LIBK_OBJS_DIR)/%.o: %.asm
	$(PROGRESS) "NASM" $<
	mkdir -p $(dir $@)
	$(NASM) $(NASM_OPTIONS) -f elf64 $< -o $@

$(LIBK_OBJECTS): CFLAGS += -D__is_libk
$(LIBK_OBJECTS): $(LIBK_OBJS_DIR)/%.o: %.c
	$(PROGRESS) "CC" $<
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBC_ASM_OBJECTS): $(LIBC_OBJS_DIR)/%.o: %.asm
	$(PROGRESS) "NASM" $<
	mkdir -p $(dir $@)
	$(NASM) $(NASM_OPTIONS) -f elf64 $< -o $@

$(LIBC_OBJECTS): CFLAGS += -D__is_libc
$(LIBC_OBJECTS): $(LIBC_OBJS_DIR)/%.o: %.c
	$(PROGRESS) "CC" $<
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBC): $(DIST_DIR) $(LIBC_ASM_OBJECTS) $(LIBC_OBJECTS)
	$(PROGRESS) "AR" $@
	$(AR) rcs $@ $(LIBC_ASM_OBJECTS) $(LIBC_OBJECTS)

libc: ## build the libc (userland)
libc: $(LIBC)
.PHONY: libc

$(KERNEL_CONSOLE_FONT): $(ARCH_BUILD_DIR)
	$(PROGRESS) "LD" $@
	cp $(KERNEL_CONSOLE_FONT_PATH) $(ARCH_BUILD_DIR)/console.sfn.gz
	gunzip -f $(ARCH_BUILD_DIR)/console.sfn.gz
	# We have to do this because we cannot control the symbol name generated by
	# `ld`. We get a pretty nice name by cd'ing into the build directory.
	cd $(ARCH_BUILD_DIR) && $(LD) -m elf_x86_64 -r -b binary -o ../../$@ console.sfn

console-font: ## compile the (default) kernel console font
console-font: $(KERNEL_CONSOLE_FONT)
.PHONY: console-font

$(INITRD): $(INITRD_DIR) userland
	$(PROGRESS) "TAR" $@
	cp -R userland/bin $(INITRD_DIR)
	echo "$(OS_NAME) ($(ARCH)) build info" > $(INITRD_DIR)/info
	echo "" >> $(INITRD_DIR)/info
	echo "hash: $(GIT_HASH)" >> $(INITRD_DIR)/info
	echo "date: $(shell date)" >> $(INITRD_DIR)/info
	echo "mode: $(BUILD_MODE)" >> $(INITRD_DIR)/info
	echo "" >> $(INITRD_DIR)/info
	echo "compiler: $(shell $(CC) --version | head -n 1)" >> $(INITRD_DIR)/info
	echo "" >> $(INITRD_DIR)/info
	echo "CFLAGS: $(CFLAGS)" >> $(INITRD_DIR)/info
	echo "INCLUDES: $(INCLUDES)" >> $(INITRD_DIR)/info
	cd $(INITRD_DIR) && tar -cf ../$@ *

# We mark this target as .PHONY to write the file every time.
$(GRUB_CFG): $(GRUB_DIR)
	echo "$$GRUB_CFG_BODY" > $@
.PHONY: $(GRUB_CFG)

$(ISO): $(DIST_DIR) $(KERNEL) $(INITRD) $(GRUB_CFG)
	$(PROGRESS) GRUB $@
	$(TOOLS_DIR)/grub-mkrescue -o $@ $(ISO_DIR)

release: ## build the OS in release mode
release: $(ISO)
.PHONY: release

run: ## run the OS in release mode
run: release
	$(PROGRESS) "RUN" $(ISO)
	$(QEMU) -cdrom $(ISO) $(QEMU_OPTIONS)
.PHONY: run

debug: ## build the OS in debug mode
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: BUILD_MODE = debug
debug: GRUB_DEFAULT = 1
debug: $(ISO)
.PHONY: debug

run-debug: ## run the OS in debug mode
run-debug: BUILD_MODE = debug
run-debug: QEMU_OPTIONS += -d guest_errors,unimp --no-reboot
run-debug: debug
	$(PROGRESS) "RUN" $(ISO)
	$(QEMU) -cdrom $(ISO) $(QEMU_OPTIONS)
.PHONY: run-debug

run-test: ## run the OS in test mode
run-test: BUILD_MODE = test
run-test: QEMU_OPTIONS += -curses
run-test: GRUB_KERNEL_CMDLINE = /bin/userland-testsuite
run-test: run
.PHONY: run-test

clean: ## remove build artifacts
	$(PROGRESS) "CLEAN"
	rm -rf $(ARCH_BUILD_DIR) $(INITRD_DIR)/info $(INITRD_DIR)/bin/ userland/bin/ userland/local-build/
.PHONY: clean

fmt: ## automatically format the code with clang-format
	find . -path ./external -prune -false -o -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format$(LLVM_SUFFIX) -style=file -i "{}" ";"
.PHONY: fmt

gdb: ## build, run the OS in debug mode and enable GDB
gdb: BUILD_MODE = debug
gdb: QEMU_OPTIONS += -s -S --no-reboot
gdb: debug run
.PHONY: gdb

userland: ## compile the userland programs (statically linked to libc)
userland: libc
	@for userland_program in $(shell find userland/* -type d -not \( -path userland/bin -o -path userland/local-build \)); do \
	    $(MAKE) -C $$userland_program OS_NAME="$(OS_NAME)" ARCH="$(ARCH)" ENABLE_USERLAND_DEBUG=$(ENABLE_USERLAND_DEBUG) ; \
	done
.PHONY: userland

test: ## run unit tests
test: CFLAGS_WITHOUT_TARGET := $(filter-out --target=x86_64,$(CFLAGS))
test: CFLAGS = $(CFLAGS_WITHOUT_TARGET)
test: CFLAGS += -fPIC -g3 -fsanitize=undefined -fsanitize=address
test: CFLAGS_FOR_TESTS += -g3 -fsanitize=undefined -fsanitize=address
test: CFLAGS_FOR_TESTS += -DENABLE_LOGS_FOR_TESTS -DTEST_ENV
test: CFLAGS_FOR_TESTS += -I./test/ -I./src/ -I./src/arch/$(ARCH)/
test: libc
	# libc
	mkdir -p $(ARCH_BUILD_DIR)/libc/string
	for file in $(LIBC_TEST_FILES); do \
		echo ; \
		$(CC) -shared $(LIBC_OBJS_DIR)/src/$$file.o -o $(ARCH_BUILD_DIR)/$$file.so ; \
		$(CC) -g3 -fsanitize=undefined -fsanitize=address -I./test/ test/$$file.c -o $(ARCH_BUILD_DIR)/$$file ; \
		LD_PRELOAD=./$(ARCH_BUILD_DIR)/$$file.so ./$(ARCH_BUILD_DIR)/$$file || exit 1 ; \
	done
	# fs/vfs
	$(CC) $(CFLAGS_FOR_TESTS) -I./test/proxies/ -o $(ARCH_BUILD_DIR)/vfs test/fs/vfs.c src/arch/$(ARCH)/fs/vfs.c
	./$(ARCH_BUILD_DIR)/vfs
	# fs/tar
	$(CC) $(CFLAGS_FOR_TESTS) -o $(ARCH_BUILD_DIR)/tar test/fs/tar.c src/arch/$(ARCH)/fs/tar.c src/arch/$(ARCH)/fs/vfs.c
	./$(ARCH_BUILD_DIR)/tar
	# fs/proc
	$(CC) $(CFLAGS_FOR_TESTS) -I./test/proxies/ -o $(ARCH_BUILD_DIR)/proc test/fs/proc.c src/arch/$(ARCH)/fs/proc.c src/arch/$(ARCH)/fs/vfs.c
	./$(ARCH_BUILD_DIR)/proc
	# fs/sock
	$(CC) $(CFLAGS_FOR_TESTS) -o $(ARCH_BUILD_DIR)/sock test/fs/sock.c src/arch/$(ARCH)/fs/sock.c src/arch/$(ARCH)/fs/vfs.c
	./$(ARCH_BUILD_DIR)/sock
	# mmu/frame
	$(CC) $(CFLAGS_FOR_TESTS) -Wformat=0 -I./test/proxies/ -o $(ARCH_BUILD_DIR)/frame test/mmu/frame.c src/arch/$(ARCH)/mmu/frame.c src/arch/$(ARCH)/core/multiboot.c src/arch/$(ARCH)/mmu/bitmap.c
	./$(ARCH_BUILD_DIR)/frame
	# config/inish
	$(CC) $(CFLAGS_FOR_TESTS) -I./test/proxies/ -o $(ARCH_BUILD_DIR)/inish test/config/inish.c src/arch/$(ARCH)/config/inish.c
	./$(ARCH_BUILD_DIR)/inish
	# mmu/bitmap
	$(CC) $(CFLAGS_FOR_TESTS) -o $(ARCH_BUILD_DIR)/bitmap test/mmu/bitmap.c src/arch/$(ARCH)/mmu/bitmap.c
	./$(ARCH_BUILD_DIR)/bitmap
	# mmu/paging
	$(CC) $(CFLAGS_FOR_TESTS) -Wformat=0 -I./test/proxies/ -o $(ARCH_BUILD_DIR)/paging test/mmu/paging.c src/arch/$(ARCH)/mmu/paging.c src/arch/$(ARCH)/core/multiboot.c src/arch/$(ARCH)/mmu/frame.c src/arch/$(ARCH)/mmu/bitmap.c src/arch/$(ARCH)/core/register.c
	./$(ARCH_BUILD_DIR)/paging
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

build-docker-image-for-circle:
	docker build . -f .circleci/images/circle/Dockerfile -t willdurand/willos-circle:latest
.PHONY: build-docker-image-for-circle

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' Makefile* | awk '{line = gensub(/^Makefile(-(.+)\.include)?:/, "ARCH=\\2 ", "g", $$0); gsub(/^ARCH= /, "", line); print line}' | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
