kernel  = kernel.bin
linker  = linker.ld
objects := $(patsubst %.asm,%.o,$(wildcard *.asm))

kernel: $(kernel)
.PHONY: kernel

$(kernel): $(objects)
	ld --nmagic --output=$@ --script=$(linker) $<

$(objects): %.o: %.asm
	nasm -f elf64 $<

clean:
	rm -f $(objects) $(kernel)
.PHONY: clean
