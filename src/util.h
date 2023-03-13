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
 * collect_methods collects n methods into a character array
 * NOTE: The variadic arguments here use the sentinel variant; the list must
 * be punctuated with NULL
 *
 * @param method
 * @param ...
 * @return array_t*
 */
array_t *collect_methods(http_method_t method, ...);

/**
 * split splits a string on all instances of a delimiter. Sets errno if
 * provided null or otherwise invalid argument(s).
 *
 * @param str The string to split
 * @param delimiter The delimiter on which to split `str`
 * @return array_t* An array of matches, if any; NULL if erroneous
 */
array_t *split(const char *str, const char *delimiter);

/**
 * index_of returns the index of a character `target`
 * as it exists in a character array `str`.
 * Sets errno if provided null or otherwise invalid argument(s).
 *
 * @param str The character array, or "haystack"
 * @param target The target, or "needle"
 *
 * @return int The index of `target` qua `str`, or -1 if not extant
 */
int index_of(const char *str, const char *target);

/**
 * substr finds and returns the substring between
 * indices `start` and `end` for a given string `str`. Sets errno if provided
 * null or otherwise invalid argument(s).
 *
 * @param str The string in which to find a substring
 * @param start The starting index, always inclusive
 * @param end The ending index, only inclusive if specified
 * @param inclusive A flag indicating whether the substring match should be
 * end-inclusive
 * @return char* The matching substring, or NULL if erroneous
 */
char *substr(const char *str, int start, int end, bool inclusive);

#endif /* UTIL_H */
