#include <kmain.h>

#include <core/isr.h>
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
#include <time/clock.h>
#include <time/timer.h>

void run_initcalls();
void init_fs(uintptr_t initrd_addr);

char* saved_cmdline = NULL;

void kmain_early_start()
{
  INFO("\n\033[1;34m%s\033[0m\n", KERNEL_ASCII);
  INFO("%s %s (%s) for %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       GIT_HASH,
       KERNEL_TARGET);
}

void run_initcalls()
{
  DEBUG("initcalls: __initcall_start=%p __initcall_end=%p",
        (void*)&__initcall_start,
        (void*)&__initcall_end);

  if ((void*)&__initcall_start == NULL) {
    DEBUG("%s", "initcalls: nothing to initialize");
    return;
  }

  initcall_t* call_fn = (initcall_t*)&__initcall_start;

  do {
    (*call_fn)();
    call_fn++;
  } while (call_fn < (initcall_t*)&__initcall_end);
}

void init_fs(uintptr_t initrd_addr)
{
  vfs_init();

  if (initrd_addr) {
    INFO("fs: mount tarfs (init ramdisk) - addr=%#lx", initrd_addr);
    inode_t initrd = vfs_mount("/", tar_fs_create(initrd_addr));

    if (!initrd) {
      WARN("%s", "fs: failed to mount tarfs (init ramdisk)");
    }
  }

  INFO("%s", "fs: mount devfs");
  if (!dev_fs_init()) {
    WARN("%s", "fs: failed to mount devfs");
  }

  INFO("%s", "fs: mount devfs");
  if (!proc_fs_init()) {
    WARN("%s", "fs: failed to mount devfs");
  }

  INFO("%s", "fs: mount sockfs");
  if (!sock_fs_init()) {
    WARN("%s", "fs: failed to mount sockfs");
  }
}

void kmain_start(uintptr_t initrd_addr, const char* cmdline)
{
  if (cmdline == NULL) {
    PANIC("invalid cmdline passed to kmain_start()");
  }
  INFO("kmain: cmdline is '%s'", cmdline);
  // Keep a copy of the original command line.
  saved_cmdline = strdup(cmdline);

  if (saved_cmdline == NULL) {
    PANIC("failed to copy cmdline, is heap memory available?");
  }

  isr_init();
  timer_init();
  clock_init();
  syscall_init();

  init_fs(initrd_addr);

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
    INFO("kmain: loading %s...", argv[0]);

    kshell(argc, argv);
  } else {
    INFO("kmain: switching to usermode... (%s)", argv[0]);

    k_execv(argv[0], argv);
  }

  PANIC("unexpectedly reached end of kmain_start()");
}
