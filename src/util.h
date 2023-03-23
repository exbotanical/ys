#ifndef UTIL_H
#define UTIL_H

#include "libhttp.h"

// ENUM_NAME extracts the name of an enum as a string literal
#define ENUM_NAME(x) [x] = #x

static const char NULL_TERM = '\0';

/**
 * safe_itoa safely converts an int into a string
 *
 * @param x
 * @return char*
 */
char *safe_itoa(int x);

/**
 * split splits a string on all instances of a delimiter.
 *
 * @param str The string to split
 * @param delimiter The delimiter on which to split `str`
 * @return array_t* An array of matches, if any; NULL if erroneous
 */
array_t *split(const char *str, const char *delimiter);

/**
 * str_join joins all strings stored in an array_t thereof with the delimiter
 * `delim`. `delim` is not appended to the end of the last string
 *
 * @param strarr
 * @param delim
 * @return char*
 */
char *str_join(array_t *strarr, const char *delim);

#endif /* UTIL_H */
