ifeq ($(shell uname -s),Darwin)
	CC = x86_64-pc-elf-gcc
else
	CC = gcc
endif

ROOT_DIR = ../..
BIN_DIR  = ../bin
TARGET   = $(BIN_DIR)/init

# Add `-no-pie` to disable the PIE feature that causes `gcc` to not create an
# executable if needed, see: https://access.redhat.com/blogs/766093/posts/1975793
CFLAGS = -Wl,-emain -Wall -pedantic -std=c11 -O0 -ffreestanding -nostdlib \
				 -D__is_libc \
				 -fno-builtin -fstack-protector-all -I $(ROOT_DIR)/src/include/ -I $(ROOT_DIR)/libs/
LIBS   = $(ROOT_DIR)/build/libc-$(OS_NAME).a

default: $(TARGET)

$(TARGET): *.c
	mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
