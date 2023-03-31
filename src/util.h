#ifndef UTIL_H
#define UTIL_H

#include "libhttp.h"

// ENUM_NAME extracts the name of an enum as a string literal
#define ENUM_NAME(x) [x] = #x

extern const char NULL_TERM;

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
 * @param s The string to split
 * @param delim The delimiter on which to split `str`
 * @return array_t* An array of matches, if any; NULL if erroneous
 */
array_t *split(const char *s, const char *delim);

/**
 * str_join joins all strings stored in an array_t thereof with the delimiter
 * `delim`. `delim` is not appended to the end of the last string
 *
 * @param sarr
 * @param delim
 * @return char*
 */
char *str_join(array_t *sarr, const char *delim);

/**
 * str_cut slices s around the first instance of sep, returning the text before
 * and after sep in a 2-element array. If sep does not appear in s, str_cut
 * returns NULL.
 * TODO: test
 * @param s
 * @param sep
 * @return array_t*
 */
array_t *str_cut(const char *s, const char *sep);

/**
 * str_contains_ctl_char tests whether s contains any ASCII control character
 *
 * @param s
 * @return true
 * @return false
 */
bool str_contains_ctl_char(const char *s);

/**
 * ishex tests whether a character is valid hex
 *
 * @param c
 * @return true
 * @return false
 */
bool ishex(char c);

/**
 * unhex converts the given char `c` into its hex integer equivalent
 * e.g. 'f' -> 15
 *
 * @param c
 * @return char
 */
char unhex(char c);

/**
 * Convert a char `c` into a null-terminated char*
 *
 * @param c
 * @return char*
 *
 * TODO: test
 */
char *tostr(char c);

/**
 * is_ascii tests whether a given character c is a printable ASCII character in
 * the range of 32 - 127
 *
 * @param c
 * @return true if the character is printable
 * @return false if the character is non-printable
 *
 * TODO: test
 */
bool is_ascii(char c);

#endif /* UTIL_H */
