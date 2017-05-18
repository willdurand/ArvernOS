CC   = /usr/local/bin/x86_64-pc-elf-gcc
LD   = /usr/local/bin/x86_64-pc-elf-ld
AR   = /usr/local/bin/x86_64-pc-elf-ar
NASM = nasm

kernel  = isofiles/boot/kernel.bin
linker  = linker.ld
iso		= os.iso
lib		= libwillos.a

OBJECTS := $(patsubst %.asm,%.o,$(wildcard *.asm))
SOURCES := $(patsubst %.c,%.o,$(shell find src -name '*.c'))

CFLAGS = -W -Wall -ansi -pedantic -std=c99 -O2 -ffreestanding -nostdlib \
		 -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
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
	qemu-system-x86_64 -cdrom os.iso
.PHONY: run

clean:
	rm -f $(OBJECTS) $(SOURCES) $(kernel) $(iso) $(lib)
.PHONY: clean
