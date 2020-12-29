/** @file */
#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#define INT_SYSCALL            "int $0x80"
#define NB_SYSCALLS            13
#define SYSCALL_TEST           1
#define SYSCALL_WRITE          2
#define SYSCALL_READ           3
#define SYSCALL_GETTIMEOFDAY   4
#define SYSCALL_OPEN           5
#define SYSCALL_CLOSE          6
#define SYSCALL_REBOOT         7
#define SYSCALL_FSTAT          8
#define SYSCALL_LSEEK          9
#define SYSCALL_SOCKET         10
#define SYSCALL_SENDTO         11
#define SYSCALL_RECVFROM       12
#define SYSCALL_GETHOSTBYNAME2 13

#define SYSCALL_SET_ERRNO()                                                    \
  if (retval < 0) {                                                            \
    errno = -retval;                                                           \
    retval = -1;                                                               \
  }

// This is needed to export the kernel syscall implementations.
#ifdef __is_libk
#include <sys/k_syscall.h>
#endif

/**
 * Implements a syscall for testing purpose only. It takes a string as input
 * and prints it to the screen.
 *
 * @param s a string to print on the screen
 */
void test(const char* s);

/**
 * Implements the write syscall.
 *
 * @param fd a file descriptor ID
 * @param buf a buffer that contains the bytes to write
 * @param count the maximum number of bytes to write
 * @return the actual number of bytes write or -1 in case of an error
 */
ssize_t write(int fd, const void* buf, size_t count);

/**
 * Implements the read syscall. `read()` attempts to read up to `count` bytes
 * from file descriptor `fd` into the buffer starting at `buf`.
 *
 * @param fd a file descriptor ID
 * @param buf a buffer that receives the bytes read
 * @param count the maximum number of bytes to read
 * @return the actual number of bytes read or -1 in case of an error
 */
ssize_t read(int fd, void* buf, size_t count);

/**
 * Implements the fstat syscall.
 *
 * @param fd a file descriptor ID
 * @param statbuf a stat structure
 * @return 0 on success or -1 in case of an error
 */
int fstat(int fd, struct stat* statbuf);

/**
 * Implements the lseek syscall.
 *
 * @param fd a file descriptor ID
 * @param offset the offset value
 * @param whence a directive to reposition the offset
 * @return the value of the offset on success or -1 in case of an error
 */
off_t lseek(int fd, off_t offset, int whence);

/**
 * Implements the gettimeofday syscall.
 */
int gettimeofday(struct timeval* p, void* z);

/**
 * Implements the open syscall.
 *
 * @param pathname the name of the file to open
 * @param flags creation or file status flags
 * @return a file descriptor, i.e. a small, non-negative integer that is used
 * in subsequent system calls
 */
int open(const char* pathname, uint32_t flags);

/**
 * Implements the close syscall.
 *
 * @param fd a file descriptor ID
 * @return zero on success, -1 otherwise
 */
int close(int fd);

/**
 * Implements the reboot syscall.
 *
 * @param command a reboot command
 * @return when `command` is `REBOOT_CMD_RESTART`, it does not return. -1 is
 * returned in case of an error, 0 otherwise.
 */
int reboot(int command);

/**
 * Implements the socket syscall.
 *
 * @param domain the communication domain (we only support `AF_INET` so far)
 * @param type the communication semantics (we only support `SOCK_DGRAM` so far)
 * @param protocol the particular protocol to be used with the socket
 * @return a file descriptor on success, -1 otherwise
 */
int socket(int domain, int type, int protocol);

/**
 * Implements the sendto syscall.
 *
 * @param sockfd socket descriptor ID
 * @param buf the data to send
 * @param len the length of the data to send
 * @param flags some flags
 * @param dst_addr the destination address
 * @param addrlen the size of the destination address structure
 * @return the number of bytes sent to the destination
 */
ssize_t sendto(int sockfd,
               const void* buf,
               size_t len,
               int flags,
               const struct sockaddr* dst_addr,
               socklen_t addrlen);

/**
 * Implements the recvfrom syscall.
 *
 * @param sockfd socket descriptor ID
 * @param buf the receiving buffer
 * @param len the size of the receiving buffer
 * @param flags some flags
 * @param src_addr the address of the sender
 * @param addrlen the size of the sender address structure
 * @return the number of bytes actually received
 */
ssize_t recvfrom(int sockfd,
                 void* buf,
                 size_t len,
                 int flags,
                 struct sockaddr* src_addr,
                 socklen_t* addrlen);

/**
 * Implements the gethostbyname2 syscall. Most existing systems don't have such
 * a syscall but we do and it performs a DNS lookup.
 *
 * @param name a hostname
 * @param in the receiving Internet address
 * @return `0` on success, a non-zero value otherwise
 */
int gethostbyname2(const char* name, struct in_addr* in);

#endif
