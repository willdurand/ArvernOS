/** @file */
#ifndef ARCH_KSHELL_H
#define ARCH_KSHELL_H

/**
 * This function is called by the `selftest` built-in command in `kshell`.
 * Architecture-specific runtime checks can be implemented in this function.
 *
 * @return `0` on success, a non-zero exit code otherwise
 */
int arch_selftest();

#endif
