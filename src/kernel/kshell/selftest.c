#include <kshell/kshell.h>

#include <arch/kshell.h>
#include <fs/vfs.h>
#include <panic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

void print_selftest_header(const char* name)
{
  printf("\n\033[1;33m[%s]\033[0m\n", name);
}

void selftest()
{
  print_selftest_header("kernel syscalls");
  // This does not call the syscall with an interrupt anymore, it directly uses
  // `k_test()` under the hood.
  test("kshell");

  print_selftest_header("kernel stacktrace");
  arch_kernel_dump_stacktrace();

  print_selftest_header("memory");
  printf("simple test with malloc()/free(): ");
  char* str = (void*)0x42;
  printf("pointer before malloc() = %p\n", str);
  int str_len = 9;
  str = (char*)malloc(str_len * sizeof(char));

  if (str == 0) {
    printf("failed\n");
  } else {
    printf("success! p=%p", str);
    strncpy(str, "it works", str_len);
    printf(" and value is: %s\n", str);
    free(str);
  }

  printf("now allocating a large chunk of memory... ");
  printf("Pointer before malloc() = %p\n", str);
  str = (char*)malloc(1024 * 1024 * 5 * sizeof(char));

  if (str == 0) {
    printf("failed\n");
  } else {
    printf("success!\n");
    free(str);
  }

  print_selftest_header("filesystem");
  inode_t debug = vfs_namei("/dev/debug");
  if (debug == NULL) {
    printf("/dev/debug not found\n");
  } else {
    const char* message = "This message should be written to the console.\n";
    vfs_write(debug, (void*)message, strlen(message), 0);
  }

  print_selftest_header(ARCH);
  arch_selftest();

  printf("\ndone.\n");
}
