#include <kmain.h>

#include <core/isr.h>
#include <fs/dev.h>
#include <fs/proc.h>
#include <fs/sock.h>
#include <fs/tar.h>
#include <fs/vfs.h>
#include <initcall.h>
#include <kshell/kshell.h>
#include <logging.h>
#include <osinfo.h>
#include <panic.h>
#include <proc/task.h>
#include <stdio.h>
#include <string.h>
#include <sys/k_syscall.h>
#include <time/clock.h>
#include <time/timer.h>

void initcall_init();
void fs_init(uintptr_t initrd_addr);

char* saved_cmdline = NULL;

void kmain_early_start()
{
  INFO("\n\033[1;34m%s\033[0m\n", KERNEL_ASCII);
  INFO("%s %s (%s) for %s has started",
       KERNEL_NAME,
       KERNEL_VERSION,
       KERNEL_GIT_HASH,
       KERNEL_TARGET);
}

void initcall_init()
{
  DEBUG("initcalls: __initcall_start=%p __initcall_end=%p",
        (void*)&__initcall_start,
        (void*)&__initcall_end);

  if ((void*)&__initcall_start == NULL) {
    DEBUG("%s", "initcalls: nothing to initialize");
    return;
  }

  initcall_foreach(call_fn)
  {
    int retval = (*call_fn)();

    if (retval != 0) {
      PANIC("initcalls: call_fn=%p failed with retval=%d", *call_fn, retval);
    }
  }
}

void fs_init(uintptr_t initrd_addr)
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

  INFO("%s", "fs: mount procfs");
  if (!proc_fs_init()) {
    WARN("%s", "fs: failed to mount procfs");
  }

  INFO("%s", "fs: mount sockfs");
  if (!sock_fs_init()) {
    WARN("%s", "fs: failed to mount sockfs");
  }
}

void kinit()
{
  int argc = 0;
  char* _cmdline = strdup(saved_cmdline);
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
  _cmdline = strdup(saved_cmdline);
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
    // TODO: create task

    // INFO("kmain: switching to usermode... (%s)", argv[0]);
    //
    // k_execv(argv[0], argv);
    WARN("cannot execute: %s", saved_cmdline);
  }

  k_exit(EXIT_FAILURE);
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

  fs_init(initrd_addr);

  initcall_init();

  task_init("idle");

  int res = task_create("kinit", KERNEL_TASK, (uintptr_t)&kinit);
  if (res < 0) {
    printf("kernel: error while starting kinit task\n");
  }

  while (1) {
    task_schedule();
  }

  PANIC("unexpectedly reached end of kmain_start()");
}
