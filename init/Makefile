CC   ?= gcc
LD   ?= ld
AR   ?= ar
NASM ?= nasm

ifeq ($(shell uname -s),Darwin)
	CC = x86_64-pc-elf-gcc
	LD = x86_64-pc-elf-ld
	AR = x86_64-pc-elf-ar
endif

TARGET    = init
ROOT_DIR  = ..
BUILD_DIR = .
CFLAGS    = -Wl,-emain -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib -I $(ROOT_DIR)/src/include
LIBS      = $(ROOT_DIR)/build/libc-willOS.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(TARGET).o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf $(TARGET) *.o
.PHONY: clean
