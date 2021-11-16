/** @file */
#ifndef KSHELL_H
#define KSHELL_H

#include <stdlib.h>

/// The number of characters to keep in the command line buffer.
#define READLINE_SIZE 256
/// The shell prompt.
#define PROMPT "kshell> "

/**
 * Initializes the kernel shell.
 */
void kshell_init(int argc, char* argv[]);

/**
 * Runs the kernel shell.
 */
void kshell_run();

void cat(int argc, char* argv[]);
void exec(int argc, char* argv[]);
void host(int argc, char* argv[]);
void ls(int argc, char* argv[]);
void net();
void ntp(int argc, char* argv[]);
void overflow();
void ping(int argc, char* argv[]);
void power_off();
void selftest();

#endif
