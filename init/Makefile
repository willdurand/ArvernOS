CC ?= gcc

ifeq ($(shell uname -s),Darwin)
	CC = x86_64-pc-elf-gcc
endif

CFLAGS = -Wl,-emain -W -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
				 -fno-builtin -fno-stack-protector -I ../src/include/
LIBS   = ../build/libc-willOS.a

default: init

init: init.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f init *.o
.PHONY: clean
