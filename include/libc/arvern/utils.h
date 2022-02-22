/** @file */
#ifndef ARVERN_UTILS_H
#define ARVERN_UTILS_H

/// Marks a variable as unused so that the compiler does not raise a warning
/// and we are aware of this variable not being used on purpose.
#define UNUSED(x) (void)(x)

/// Marks a variable as potentially unused.
#define MAYBE_UNUSED(x) UNUSED(x)

#endif
