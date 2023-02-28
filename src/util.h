#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#include "libhash/libhash.h"

static const char NULL_TERM = '\0';

/**
 * @brief Safely converts an int to a string.
 *
 * @param x
 * @return char*
 */
char *safe_itoa(int x);

/**
 * @brief Essentially a safe, variadic snprintf.
 *
 * @param fmt
 * @param ...
 * @return char*
 */
char *fmt_str(char *fmt, ...);

#endif /* UTIL_H */
