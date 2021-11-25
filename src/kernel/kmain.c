#include <kmain.h>

#include <fs/dev.h>
#include <fs/proc.h>
#include <fs/sock.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <init.h>
#include <kshell/kshell.h>
#include <logging.h>
#include <osinfo.h>
#include <panic.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>

void run_initcalls();

char* saved_cmdline = NULL;

void print_step(const char* msg)
{
  printf("kernel: %-66s", msg);
}

void print_ok()
{
  printf("  [");
  printf("\033[0;32mOK\033[0m");
  printf("]\n");
}

void print_ko()
{
  printf("[");
  printf("\033[0;31mFAIL\033[0m");
  printf("]\n");
}

void kmain_print_banner()
{
  INFO("%s %s - %s (%s) / Built on: %s at %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       ARCH,
       GIT_HASH,
       KERNEL_DATE,
       KERNEL_TIME);

  printf("\033[1;34m%s\033[0m\n", KERNEL_ASCII);
  printf("%s %s - %s (%s) / Built on: %s at %s\n\n",
         KERNEL_NAME,
         KERNEL_VERSION,
         ARCH,
         GIT_HASH,
         KERNEL_DATE,
         KERNEL_TIME);
}

void run_initcalls()
{
  DEBUG("initcalls: __initcall_start=%p __initcall_end=%p",
        &__initcall_start,
        &__initcall_end);

  if (__initcall_start == 0) {
    return;
  }

  initcall_t* call_fn = &__initcall_start;

  do {
    (*call_fn)();
    call_fn++;
  } while (call_fn < &__initcall_end);
}

void kmain_init_fs(uintptr_t initrd_addr)
{
  print_step("initializing virtual file system");
  if (vfs_init()) {
    print_ok();
  } else {
    print_ko();
  }

  if (initrd_addr) {
    print_step("mounting tarfs (init ramdisk)");
    inode_t initrd = vfs_mount("/", tar_fs_create(initrd_addr));

    if (initrd) {
      print_ok();
    } else {
      print_ko();
    }
  }

  print_step("mounting devfs");
  if (dev_fs_init()) {
    print_ok();
  } else {
    print_ko();
  }

  print_step("mounting procfs");
  if (proc_fs_init()) {
    print_ok();
  } else {
    print_ko();
  }

  print_step("mounting sockfs");
  if (sock_fs_init()) {
    print_ok();
  } else {
    print_ko();
  }
}

void kmain_start(const char* cmdline)
{
  if (cmdline == NULL) {
    PANIC("invalid cmdline passed to kmain_start()");
  }
  DEBUG("cmdline=%s", cmdline);
  // Keep a copy of the original command line.
  saved_cmdline = strdup(cmdline);

  run_initcalls();

  int argc = 0;
  char* _cmdline = strdup(cmdline);
  char* curr = strtok(_cmdline, " ");
  while (curr != NULL) {
    // We exclude command line arguments that contain a dot like
    // `module.param=value`.
    if (strchr(curr, '.') == 0) {
      argc++;
    }
    curr = strtok(NULL, " ");
  }
  free(_cmdline);

  char** argv = (char**)malloc(sizeof(char*) * (argc + 1));

  int _argc = 0;
  _cmdline = strdup(cmdline);
  curr = strtok(_cmdline, " ");
  while (curr != NULL) {
    if (strchr(curr, '.') == 0) {
      argv[_argc++] = strdup(curr);
    }
    curr = strtok(NULL, " ");
  }
  argv[_argc] = NULL;
  free(_cmdline);

  if (strcmp(argv[0], "kshell") == 0) {
    printf("kernel: loading %s...\n", argv[0]);
    INFO("kernel: loading %s...", argv[0]);

    kshell(argc, argv);
  } else {
    printf("kernel: switching to usermode... (%s)\n", argv[0]);
    INFO("kernel: switching to usermode... (%s)", argv[0]);

    k_execv(argv[0], argv);
  }

  PANIC("unexpectedly reached end of kmain_start()");
}
