ifeq ($(shell uname -s),Darwin)
	CC ?= x86_64-pc-elf-gcc
else
	CC ?= gcc
endif

# Add `-no-pie` to disable the PIE feature that causes `gcc` to not create an
# executable if needed, see: https://access.redhat.com/blogs/766093/posts/1975793
CFLAGS = -Wl,-emain -Wall -pedantic -std=c11 -O2 -ffreestanding -nostdlib \
				 -D__is_libc \
				 -fno-builtin -fno-stack-protector -I ../src/include/ -I ../libs/
LIBS   = ../build/libc-willOS.a

default: init

init: init.c cdate.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f init *.o
.PHONY: clean
