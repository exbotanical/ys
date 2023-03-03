#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#include "array.h"
#include "libhash/libhash.h"
#include "libhttp.h"

#define EP(x) [x] = #x

static const char NULL_TERM = '\0';

/**
 * @brief Safely converts an int to a string.
 *
 * @param x
 * @return char*
 */
char *safe_itoa(int x);

#endif /* UTIL_H */
