#include <sys/k_syscall.h>

#include <arch/sys.h>
#include <arvern/utils.h>
#include <sys/logging.h>
#include <sys/syscall.h>

typedef void (*syscall_ptr_t)(void);

syscall_ptr_t syscall_handlers[400] = {
  [0 ... 399] = (syscall_ptr_t)k_not_implemented,
};

static const char* syscall_names[400] = {
  [0 ... 399] = "(unknown)",
};

static int k_return_zero()
{
  uint64_t num = 0;
  MAYBE_UNUSED(num);

#ifdef __x86_64__
  __asm__("mov %%rax, %0" : "=r"(num) : /* no input */);
#elif __aarch64__
  __asm__("mov x8, %0" : "=r"(num) : /* no input */);
#endif

  DEBUG("called syscall stub for %s (0x%02X)", syscall_names[num], num);

  return 0;
}

int k_not_implemented()
{
  uint64_t num = 0;
  MAYBE_UNUSED(num);

#ifdef __x86_64__
  __asm__("mov %%rax, %0" : "=r"(num) : /* no input */);
#elif __aarch64__
  __asm__("mov x8, %0" : "=r"(num) : /* no input */);
#endif

  DEBUG("called unimplemented syscall %s (0x%02X)", syscall_names[num], num);

  return -ENOSYS;
}

void syscall_init()
{
  INFO("%s", "sys: initialize syscalls");

  MAYBE_UNUSED(syscall_names);
  MAYBE_UNUSED(k_return_zero);

  syscall_handlers[SYSCALL_TEST] = (syscall_ptr_t)k_test;
  syscall_handlers[SYSCALL_WRITE] = (syscall_ptr_t)k_write;
  syscall_handlers[SYSCALL_READ] = (syscall_ptr_t)k_read;
  syscall_handlers[SYSCALL_GETTIMEOFDAY] = (syscall_ptr_t)k_gettimeofday;
#ifndef __aarch64__
  syscall_handlers[SYSCALL_OPEN] = (syscall_ptr_t)k_open;
#endif
  syscall_handlers[SYSCALL_CLOSE] = (syscall_ptr_t)k_close;
  syscall_handlers[SYSCALL_REBOOT] = (syscall_ptr_t)k_reboot;
  syscall_handlers[SYSCALL_FSTAT] = (syscall_ptr_t)k_fstat;
  syscall_handlers[SYSCALL_LSEEK] = (syscall_ptr_t)k_lseek;
  syscall_handlers[SYSCALL_SOCKET] = (syscall_ptr_t)k_socket;
  syscall_handlers[SYSCALL_SENDTO] = (syscall_ptr_t)k_sendto;
  syscall_handlers[SYSCALL_RECVFROM] = (syscall_ptr_t)k_recvfrom;
  syscall_handlers[SYSCALL_GETHOSTBYNAME2] = (syscall_ptr_t)k_gethostbyname2;
  syscall_handlers[SYSCALL_EXECV] = (syscall_ptr_t)k_execv;
  syscall_handlers[SYSCALL_GETPID] = (syscall_ptr_t)k_getpid;
  syscall_handlers[SYSCALL_EXIT] = (syscall_ptr_t)k_exit;
  syscall_handlers[SYSCALL_OPENAT] = (syscall_ptr_t)k_openat;

#ifdef SYSCALL_IOCTL
  syscall_handlers[SYSCALL_IOCTL] = (syscall_ptr_t)k_ioctl;
#endif
#ifdef SYSCALL_WRITEV
  syscall_handlers[SYSCALL_WRITEV] = (syscall_ptr_t)k_writev;
#endif
#ifdef SYSCALL_DUP2
  syscall_handlers[SYSCALL_DUP2] = (syscall_ptr_t)k_dup2;
#endif
#ifdef SYSCALL_GETEUID
  // TODO: Implement handler for `geteuid`.
  syscall_handlers[SYSCALL_GETEUID] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_GETEUID] = "geteuid";
#endif
#ifdef SYSCALL_ARCH_PRCTL
  syscall_handlers[SYSCALL_ARCH_PRCTL] = (syscall_ptr_t)k_arch_prctl;
#endif
#ifdef SYSCALL_SET_TID_ADDR
  // TODO: Implement handler for `set_tid_addr`.
  syscall_handlers[SYSCALL_SET_TID_ADDR] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_SET_TID_ADDR] = "set_tid_address";
#endif
#ifdef SYSCALL_EXIT_GROUP
  syscall_handlers[SYSCALL_EXIT_GROUP] = (syscall_ptr_t)k_exit;
#endif
#ifdef SYSCALL_BRK
  // TODO: Implement handler for `brk`, maybe.
  syscall_handlers[SYSCALL_BRK] = (syscall_ptr_t)k_not_implemented;
  syscall_names[SYSCALL_BRK] = "brk";
#endif
#ifdef SYSCALL_POLL
  syscall_handlers[SYSCALL_POLL] = (syscall_ptr_t)k_poll;
#endif
#ifdef SYSCALL_MMAP
  syscall_handlers[SYSCALL_MMAP] = (syscall_ptr_t)k_mmap;
#endif
#ifdef SYSCALL_RT_SIGPROCMASK
  // TODO: Implement handler for `rt_sigprocmask`.
  syscall_handlers[SYSCALL_RT_SIGPROCMASK] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_RT_SIGPROCMASK] = "rt_sigprocmask";
#endif
#ifdef SYSCALL_NANOSLEEP
  // TODO: Implement handler for `nanosleep`.
  syscall_handlers[SYSCALL_NANOSLEEP] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_NANOSLEEP] = "nanosleep";
#endif
#ifdef SYSCALL_CONNECT
  syscall_handlers[SYSCALL_CONNECT] = (syscall_ptr_t)k_connect;
#endif
#ifdef SYSCALL_BIND
  // TODO: Implement handler for `bind`.
  syscall_handlers[SYSCALL_BIND] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_BIND] = "bind";
#endif
#ifdef SYSCALL_GETSOCKNAME
  syscall_handlers[SYSCALL_GETSOCKNAME] = (syscall_ptr_t)k_getsockname;
#endif
#ifdef SYSCALL_SETSOCKOPT
  // TODO: Implement handler for `setsockopt`.
  syscall_handlers[SYSCALL_SETSOCKOPT] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_SETSOCKOPT] = "setsockopt";
#endif
#ifdef SYSCALL_UNAME
  syscall_handlers[SYSCALL_UNAME] = (syscall_ptr_t)k_uname;
#endif
#ifdef SYSCALL_GETCWD
  syscall_handlers[SYSCALL_GETCWD] = (syscall_ptr_t)k_getcwd;
#endif
#ifdef SYSCALL_SETUID
  // TODO: Implement handler for `setuid`.
  syscall_handlers[SYSCALL_SETUID] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_SETUID] = "setuid";
#endif
#ifdef SYSCALL_SETGID
  // TODO: Implement handler for `setgid`.
  syscall_handlers[SYSCALL_SETGID] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_SETGID] = "setgid";
#endif
#ifdef SYSCALL_FCNTL
  // TODO: Implement handler for `fcntl`.
  syscall_handlers[SYSCALL_FCNTL] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_FCNTL] = "fcntl";
#endif
#ifdef SYSCALL_MUNMAP
  // TODO: Implement handler for `munmap`.
  syscall_handlers[SYSCALL_MUNMAP] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_MUNMAP] = "munmap";
#endif
#ifdef SYSCALL_RT_SIGACTION
  // TODO: Implement handler for `rt_sigaction`.
  syscall_handlers[SYSCALL_RT_SIGACTION] = (syscall_ptr_t)k_return_zero;
  syscall_names[SYSCALL_RT_SIGACTION] = "rt_sigaction";
#endif
#ifdef SYSCALL_GETRANDOM
  // TODO: Implement handler for `getrandom`.
  syscall_handlers[SYSCALL_GETRANDOM] = (syscall_ptr_t)k_not_implemented;
  syscall_names[SYSCALL_GETRANDOM] = "getrandom";
#endif

  arch_syscall_init();
}
