#ifndef UTIL_H
#define UTIL_H

#include "libhttp.h"

/**
 * Extracts the name of an enum as a string literal
 */
#define ENUM_NAME(x) [x] = #x

static const char NULL_TERM = '\0';

/**
 * @brief Safely converts an int to a string.
 *
 * @param x
 * @return char*
 */
char *safe_itoa(int x);

#endif /* UTIL_H */
