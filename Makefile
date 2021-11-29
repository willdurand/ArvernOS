ARCH           ?= x86_64
OS_NAME        = ArvernOS
BUILD_MODE     = release
GIT_HASH       := $(shell git rev-parse --short HEAD)
CMDLINE        ?= /bin/init -s
KERNEL_CMDLINE ?= kshell

include ./Makefile.include

# We (more or less) follow the PFL project structure:
# https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#intro.dirs
BUILD_DIR    = build
DATA_DIR     = data
EXTERNAL_DIR = external
INCLUDE_DIR  = include
LOG_DIR      = log
SRC_DIR      = src
TESTS_DIR    = tests
TOOLS_DIR    = tools

# Source directories.
KERNEL_SRC_DIR   = $(SRC_DIR)/kernel
KERNEL_TESTS_DIR = $(TESTS_DIR)/kernel
USERLAND_SRC_DIR = $(SRC_DIR)/userland
ARCH_SRC         = $(KERNEL_SRC_DIR)/arch/$(ARCH)
INITRD_DIR       = $(DATA_DIR)/initrd
# Build directories.
ARCH_BUILD_DIR   = $(BUILD_DIR)/$(ARCH)
DIST_DIR         = $(ARCH_BUILD_DIR)/dist
MISC_DIR         = $(ARCH_BUILD_DIR)/misc
LIBC_OBJS_DIR    = $(ARCH_BUILD_DIR)/libc-objects
LIBK_OBJS_DIR    = $(ARCH_BUILD_DIR)/libk-objects
# Files.
INITRD_TAR       = initrd.tar
KERNEL_BIN       = kernel-$(ARCH).bin
LINKER_LD        = $(ARCH_SRC)/linker.ld
SYMBOLS_TXT      = symbols.txt
KERNEL           = $(DIST_DIR)/$(KERNEL_BIN)
LIBC             = $(DIST_DIR)/libc-$(OS_NAME)-$(ARCH).a
SYMBOLS          = $(DIST_DIR)/$(SYMBOLS_TXT)
INITRD           = $(MISC_DIR)/$(INITRD_TAR)
LOG_FILE         = $(LOG_DIR)/$(ARCH)-$(BUILD_MODE).log

# More tools.
TAR = tar

CONFIG_CFLAGS += -DGIT_HASH=\"$(GIT_HASH)\"
CONFIG_CFLAGS += -DARCH=\"$(ARCH)\"

# This is the list of external libraries we use and need to build for the
# kernel (libk) and the libc.
EXTERNAL_DEPS = printf vtconsole

ifeq ($(CONFIG_USE_DLMALLOC), 1)
	EXTERNAL_DEPS += dlmalloc
	CONFIG_CFLAGS += -DCONFIG_USE_DLMALLOC
else
	EXTERNAL_DEPS += liballoc
endif

EXTERNAL_DIRS := $(addprefix $(EXTERNAL_DIR)/,$(EXTERNAL_DEPS))

# First, we gather the files for the architecture, then all the other files in
# `src/` but without the arch files.
LIBK_C_FILES := $(shell find $(ARCH_SRC) -name '*.c')
LIBK_C_FILES += $(shell find $(KERNEL_SRC_DIR) src/libc $(EXTERNAL_DIRS) -not -path "src/kernel/arch/*" -name '*.c')

LIBK_OBJECTS     := $(patsubst %.c, $(LIBK_OBJS_DIR)/%.o, $(LIBK_C_FILES))
LIBK_ASM_OBJECTS := $(patsubst %.asm, $(LIBK_OBJS_DIR)/%.o, $(shell find $(ARCH_SRC)/asm -name '*.asm'))
LIBC_OBJECTS     := $(patsubst %.c, $(LIBC_OBJS_DIR)/%.o, $(shell find src/libc $(EXTERNAL_DIRS) -not -path "$(EXTERNAL_DIR)/dlmalloc/*" -name '*.c'))
LIBC_ASM_OBJECTS := $(patsubst %.asm, $(LIBC_OBJS_DIR)/%.o, $(shell find src/libc/asm -name '*.asm'))
LIBC_TEST_FILES  := $(patsubst $(TESTS_DIR)/%.c, %, $(shell find $(TESTS_DIR)/libc -name '*.c'))

INCLUDES += -I$(INCLUDE_DIR)/libc/
INCLUDES += $(addprefix -I$(EXTERNAL_DIR)/,$(EXTERNAL_DEPS))
INCLUDES += -I$(EXTERNAL_DIR)/scalable-font2/

WERRORS += -Wall -Wextra -Werror
WERRORS += -Wformat=2
WERRORS += -Wno-null-pointer-arithmetic

CFLAGS += -O2 -std=c11 -ffreestanding -nostdinc -nostdlib -fno-builtin
CFLAGS += $(WERRORS)
CFLAGS += $(CONFIG_CFLAGS)

LD_FLAGS += --nmagic -nostdlib --gc-sections

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
	DEBUG_CFLAGS += -DENABLE_CONFIG_DEBUG
	DEBUG_CFLAGS += -DENABLE_CORE_DEBUG
	DEBUG_CFLAGS += -DENABLE_FS_DEBUG
	DEBUG_CFLAGS += -DENABLE_MMU_DEBUG
	DEBUG_CFLAGS += -DENABLE_NET_DEBUG
	DEBUG_CFLAGS += -DENABLE_PROC_DEBUG
	DEBUG_CFLAGS += -DENABLE_SYS_DEBUG
	DEBUG_CFLAGS += -DENABLE_USERLAND_DEBUG
endif

default: help

$(BUILD_DIR):
	mkdir -p $@

$(ARCH_BUILD_DIR): $(BUILD_DIR)
	mkdir -p $@

$(DIST_DIR): $(ARCH_BUILD_DIR)
	mkdir -p $@

$(MISC_DIR): $(ARCH_BUILD_DIR)
	mkdir -p $@

$(KERNEL): $(DIST_DIR) $(LIBK_ASM_OBJECTS) $(LIBK_OBJECTS) $(LINKER_LD)
	$(PROGRESS) "LD" $@
	$(LD) --output=$@ --script=$(LINKER_LD) $(LD_FLAGS) $(LIBK_ASM_OBJECTS) $(LIBK_OBJECTS) $(LINK_TO_KERNEL)
	$(NM) $@ | awk '{ print $$1, $$3 }' | sort > $(SYMBOLS)

$(LIBK_OBJECTS): CFLAGS += -D__is_libk
$(LIBK_OBJECTS): INCLUDES += -I$(INCLUDE_DIR)/kernel/ -I$(ARCH_SRC)/
$(LIBK_OBJECTS): $(LIBK_OBJS_DIR)/%.o: %.c
	$(PROGRESS) "CC" $<
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBC_OBJECTS): CFLAGS += -D__is_libc
$(LIBC_OBJECTS): $(LIBC_OBJS_DIR)/%.o: %.c
	$(PROGRESS) "CC" $<
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBC): $(DIST_DIR) $(LIBC_ASM_OBJECTS) $(LIBC_OBJECTS)
	$(PROGRESS) "AR" $@
	$(AR) rcs $@ $(LIBC_ASM_OBJECTS) $(LIBC_OBJECTS)

$(INITRD): userland
	$(PROGRESS) "TAR" $@
	cp -R $(USERLAND_SRC_DIR)/bin $(INITRD_DIR)
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
	cd $(INITRD_DIR) && $(TAR) -cf ../../$@ *

libc: ## build the libc (userland)
libc: $(LIBC)
.PHONY: libc

release: ## build the OS in release mode
release: arch-release
.PHONY: release

run-release: ## run the OS in release mode
run-release: arch-run-release
.PHONY: run-release

debug: ## build the OS in debug mode
debug: arch-debug
.PHONY: debug

run-debug: ## run the OS in debug mode
run-debug: arch-run-debug
.PHONY: run-debug

gdb: ## build, run the OS in debug mode and enable GDB
gdb: QEMU_OPTIONS += -s -S
gdb: run-debug
.PHONY: gdb

run-test: ## run the OS in test mode
run-test: arch-run-test
.PHONY: run-test

userland: ## compile the userland programs (statically linked to libc)
userland: libc
	@for userland_program in $(shell find $(USERLAND_SRC_DIR)/* -type d -not \( -path $(USERLAND_SRC_DIR)/bin -o -path $(USERLAND_SRC_DIR)/local-build \)); do \
	    $(MAKE) -C $$userland_program OS_NAME="$(OS_NAME)" ARCH="$(ARCH)" ENABLE_USERLAND_DEBUG=$(ENABLE_USERLAND_DEBUG) ; \
	done
.PHONY: userland

fmt: ## automatically format the code with clang-format
	find . -path ./external -prune -false -o -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format$(LLVM_SUFFIX) -style=file -i "{}" ";"
.PHONY: fmt

test: ## run unit tests
test: arch-test
.PHONY: test

version: ## print tool versions
version: arch-version
	$(CC) --version
	$(LD) --version
	$(AR) --version
	$(QEMU) --version
.PHONY: version

docs: ## build the docs
	rm -rf docs/*
	doxygen ./Doxyfile
.PHONY: docs

build-docker-image-for-circle:
	docker build . -f .circleci/images/circle/Dockerfile -t willdurand/arvernos-circle:latest
.PHONY: build-docker-image-for-circle

clean: ## remove build artifacts
	$(PROGRESS) "CLEAN"
	rm -rf $(ARCH_BUILD_DIR) $(INITRD_DIR)/info $(INITRD_DIR)/bin/ $(USERLAND_SRC_DIR)/bin/ $(USERLAND_SRC_DIR)/local-build/
.PHONY: clean

# Include architecture-specific makefile.
include $(ARCH_SRC)/Makefile.include

help:
	@echo "ArvernOS - available commands for arch=$(ARCH)\n"
	@grep -hE '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.PHONY: help
