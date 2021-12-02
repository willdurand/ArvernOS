/** @file */
#ifndef KSHELL_KSHELL_H
#define KSHELL_KSHELL_H

#include <stdlib.h>

/// The number of characters to keep in the command line buffer.
#define READLINE_SIZE 256
/// The shell prompt.
#define PROMPT "kshell> "

/**
 * Starts the kernel shell.
 */
void kshell(int argc, char* argv[]);

#endif
