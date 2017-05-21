CC   ?= gcc
LD   ?= ld
AR   ?= ar
NASM ?= nasm

ifeq ($(shell uname -s),Darwin)
	CC = x86_64-pc-elf-gcc
	LD = x86_64-pc-elf-ld
	AR = x86_64-pc-elf-ar
endif

kernel = isofiles/boot/kernel.bin
linker = linker.ld
iso    = os.iso
lib    = libwillos.a

OBJECTS := $(patsubst %.asm,%.o,$(shell find asm -name '*.asm'))
SOURCES := $(patsubst %.c,%.o,$(shell find src -name '*.c'))

CFLAGS = -W -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
		 -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		 -mno-red-zone \
		 -I src/include/ -I src/

default: iso

kernel: $(kernel)
.PHONY: kernel

$(kernel): $(OBJECTS) $(lib)
	$(LD) --nmagic --output=$@ --script=$(linker) $(OBJECTS) $(lib)

$(OBJECTS): %.o: %.asm
	$(NASM) -f elf64 $<

$(SOURCES): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(lib): $(SOURCES)
	$(AR) rcs $@ $^

iso: $(iso)
.PHONY: iso

$(iso): $(kernel)
	grub-mkrescue -o $@ isofiles

run: $(iso)
	qemu-system-x86_64 -cdrom $<
.PHONY: run

debug: $(iso)
	qemu-system-x86_64 -cdrom $< -serial file:/tmp/serial.log
.PHONY: run

clean:
	rm -f $(OBJECTS) $(SOURCES) $(kernel) $(iso) $(lib)
.PHONY: clean
