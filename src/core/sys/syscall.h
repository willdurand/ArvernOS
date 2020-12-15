/** @file */
#ifndef CORE_SYS_SYSCALL_H
#define CORE_SYS_SYSCALL_H

#include <core/isr.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

/// This type represents a syscall handler.
typedef void (*syscall_handler_t)(registers_t* registers);

/**
 * Initializes the syscalls management.
 */
void syscall_init();

/**
 * This is the handler attached to the interrupt used by the syscalls. It is
 * called by the interrupt handler and responsible for finding the right
 * syscall handler to call.
 *
 * @param registers the registers passed to the handler
 */
void syscall_handler(registers_t* registers);

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

// Below are the different syscall handlers.
void syscall_test(registers_t* registers);
void syscall_write(registers_t* registers);
void syscall_socket(registers_t* registers);
void syscall_recvfrom(registers_t* registers);

#endif
