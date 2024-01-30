// https://chromium.googlesource.com/chromiumos/docs/+/HEAD/constants/syscalls.md
#ifndef SYS_LINUX_COMPAT_H
#define SYS_LINUX_COMPAT_H

#ifdef __x86_64__

#define SYSCALL_READ           0
#define SYSCALL_WRITE          1
#define SYSCALL_OPEN           2
#define SYSCALL_CLOSE          3
#define SYSCALL_FSTAT          5
#define SYSCALL_POLL           7
#define SYSCALL_LSEEK          8
#define SYSCALL_MMAP           9
#define SYSCALL_BRK            12
#define SYSCALL_RT_SIGACTION   13
#define SYSCALL_RT_SIGPROCMASK 14
#define SYSCALL_IOCTL          16
#define SYSCALL_WRITEV         20
#define SYSCALL_DUP2           33
#define SYSCALL_NANOSLEEP      35
#define SYSCALL_GETPID         39
#define SYSCALL_SOCKET         41
#define SYSCALL_CONNECT        42
#define SYSCALL_SENDTO         44
#define SYSCALL_RECVFROM       45
#define SYSCALL_BIND           49
#define SYSCALL_GETSOCKNAME    51
#define SYSCALL_SETSOCKOPT     54
#define SYSCALL_EXECV          59
#define SYSCALL_EXIT           60
#define SYSCALL_UNAME          63
#define SYSCALL_FCNTL          72
#define SYSCALL_GETCWD         79
#define SYSCALL_GETTIMEOFDAY   96
#define SYSCALL_SETUID         105
#define SYSCALL_SETGID         106
#define SYSCALL_GETEUID        107
#define SYSCALL_ARCH_PRCTL     158
#define SYSCALL_REBOOT         169
#define SYSCALL_SET_TID_ADDR   218
#define SYSCALL_EXIT_GROUP     231
#define SYSCALL_OPENAT         257
#define SYSCALL_GETRANDOM      318

#elif __arm__

#define SYSCALL_EXIT         1
#define SYSCALL_READ         3
#define SYSCALL_WRITE        4
#define SYSCALL_OPEN         5
#define SYSCALL_CLOSE        6
#define SYSCALL_EXECV        11
#define SYSCALL_LSEEK        19
#define SYSCALL_GETPID       20
#define SYSCALL_BRK          45
#define SYSCALL_GETEUID      49
#define SYSCALL_IOCTL        54
#define SYSCALL_DUP2         63
#define SYSCALL_GETTIMEOFDAY 78
#define SYSCALL_REBOOT       88
#define SYSCALL_FSTAT        108
#define SYSCALL_WRITEV       146
#define SYSCALL_EXIT_GROUP   248
#define SYSCALL_SET_TID_ADDR 256
#define SYSCALL_SOCKET       281
#define SYSCALL_SENDTO       290
#define SYSCALL_RECVFROM     292
#define SYSCALL_OPENAT       322
#define SYSCALL_GETRANDOM    384

// Not available on AArch32:
//
// - SYSCALL_ARCH_PRCTL

#elif __aarch64__

#define SYSCALL_FCNTL        25
#define SYSCALL_IOCTL        29
#define SYSCALL_OPENAT       56
#define SYSCALL_CLOSE        57
#define SYSCALL_LSEEK        62
#define SYSCALL_READ         63
#define SYSCALL_WRITE        64
#define SYSCALL_WRITEV       66
#define SYSCALL_FSTAT        80
#define SYSCALL_EXIT         93
#define SYSCALL_EXIT_GROUP   94
#define SYSCALL_SET_TID_ADDR 96
#define SYSCALL_REBOOT       142
#define SYSCALL_GETTIMEOFDAY 169
#define SYSCALL_GETPID       172
#define SYSCALL_GETEUID      175
#define SYSCALL_SOCKET       198
#define SYSCALL_SENDTO       206
#define SYSCALL_RECVFROM     207
#define SYSCALL_BRK          214
#define SYSCALL_MUNMAP       215
#define SYSCALL_EXECV        221
#define SYSCALL_MMAP         222
#define SYSCALL_GETRANDOM    278

// Not available on AArch64:
//
// - SYSCALL_OPEN
// - SYSCALL_DUP2
// - SYSCALL_ARCH_PRCTL

#endif

// Not available outside ArvernOS:
#define SYSCALL_TEST           348
#define SYSCALL_GETHOSTBYNAME2 349

#endif
