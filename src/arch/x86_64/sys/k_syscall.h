/** @file */
#ifndef SYS_K_SYSCALL_H
#define SYS_K_SYSCALL_H

#include <arpa/inet.h>
#include <core/isr.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

// This is defined in `src/arch/x86_64/k_syscall.asm`.
extern void syscall_handler();

/**
 * Initializes the syscalls management.
 */
void syscall_init();

int k_open(const char* pathname, uint32_t flags);
int k_close(int fd);
ssize_t k_read(int fd, void* buf, size_t count);
off_t k_lseek(int fd, off_t offset, int whence);
int k_fstat(int fd, struct stat* statbuf);
int k_gettimeofday(struct timeval* p, void* z);
int k_reboot(int command);
ssize_t k_sendto(int sockfd,
                 const void* buf,
                 size_t len,
                 int flags,
                 const struct sockaddr* dst_addr,
                 socklen_t addrlen);
int k_socket(int domain, int type, int protocol);
ssize_t k_write(int fd, const void* buf, size_t count);
void k_test(const char* s);
ssize_t k_recvfrom(int sockfd,
                   void* buf,
                   size_t len,
                   int flags,
                   struct sockaddr* src_addr,
                   socklen_t* addrlen);
int k_gethostbyname2(const char* name, struct in_addr* in);
int k_execv(const char* path, char* const argv[]);
pid_t k_getpid();
int k_yield();

#endif
