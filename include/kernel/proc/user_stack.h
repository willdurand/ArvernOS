/** @file */
#ifndef PROC_USER_STACK_H
#define PROC_USER_STACK_H

#include <stdint.h>

/// Maximum number of arguments/env vars copied to the user stack.
#define MAX_STACK_ARGS 64

/// Alignment (in bytes) of the returned user stack pointer.
#define STACK_ALIGNMENT 16

/// End of the auxiliary vector.
#define AT_NULL 0
/// Page size.
#define AT_PAGESZ 6
/// Entry point of the program.
#define AT_ENTRY 9

/**
 * Builds the initial user stack for a program about to be executed.
 *
 * The stack is written downward from `stack_top`.
 *
 *   +--------------------------+  <- stack_top
 *   | strings                  |
 *   +--------------------------+  <- returned stack pointer
 *   | argc                     |
 *   | argv[0..argc-1]          |
 *   | NULL                     |
 *   | envp[0..]                |
 *   | NULL                     |
 *   | AT_PAGESZ, page_size     |
 *   | AT_ENTRY, entry          |
 *   | AT_NULL, 0               |
 *   +--------------------------+
 *
 * The returned pointer is 16-byte aligned.
 *
 * @param stack_top the top (high address) of the user stack
 * @param argc the number of arguments
 * @param argv the argument list
 * @param envp the environment list
 * @param entry the program entry point
 * @return the initial stack pointer, or `0` on error
 */
uintptr_t create_user_stack(uintptr_t stack_top,
                            int argc,
                            char* const argv[],
                            char* const envp[],
                            uintptr_t entry);

#endif
