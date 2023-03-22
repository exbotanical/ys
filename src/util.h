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
 * index_of returns the index of a character `target`
 * as it exists in a character array `str`.
 *
 * @param str The character array, or "haystack"
 * @param target The target, or "needle"
 *
 * @return int The index of `target` qua `str`, or -1 if not extant
 */
int index_of(const char *str, const char *target);

/**
 * substr finds and returns the substring between
 * indices `start` and `end` for a given string `str`.
 *
 * @param str The string in which to find a substring
 * @param start The starting index, always inclusive
 * @param end The ending index, only inclusive if specified
 * @param inclusive A flag indicating whether the substring match should be
 * end-inclusive
 * @return char* The matching substring, or NULL if erroneous
 */
char *substr(const char *str, int start, int end, bool inclusive);

/**
 * safe_strcasecmp performs a safe case-insensitive string comparison between
 * two strings and returns a bool indicating whether they are equal
 *
 * @param s1
 * @param s2
 * @return bool
 */
bool safe_strcasecmp(const char *s1, const char *s2);

/**
 * str_join joins all strings stored in an array_t thereof with the delimiter
 * `delim`. `delim` is not appended to the end of the last string
 *
 * @param strarr
 * @param delim
 * @return char*
 */
char *str_join(array_t *strarr, const char *delim);

/**
 * to_upper converts the given string `s` to uppercase
 *
 * @param s
 * @return char*
 */
char *to_upper(const char *s);

/**
 * str_equals returns a bool indicating whether strings s1 and s2 are completely
 * equal (case-sensitive)
 *
 * @param s1
 * @param s2
 * @return bool
 */
bool str_equals(const char *s1, const char *s2);

// TODO: doc + test
bool str_nullish(const char *s);
#endif /* UTIL_H */
