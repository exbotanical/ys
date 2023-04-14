#ifndef UTIL_H
#define UTIL_H

#include "libys.h"

// ENUM_NAME extracts the name of an enum as a string literal
#define ENUM_NAME(x) [x] = #x

extern const char NULL_TERMINATOR;

/**
 * safe_itoa safely converts an int into a string
 */
char *safe_itoa(int x);

/**
 * split splits a string on all instances of a delimiter and returns an array of
 * matches, if any, or NULL if erroneous
 */
array_t *split(const char *s, const char *delim);

/**
 * str_join joins all strings stored in an array_t thereof with the delimiter
 * `delim`. `delim` is not appended to the end of the last string
 */
char *str_join(array_t *sarr, const char *delim);

/**
 * str_cut slices s around the first instance of sep, returning the text before
 * and after sep in a 2-element array. If sep does not appear in s, str_cut
 * returns NULL.
 */
array_t *str_cut(const char *s, const char *sep);

/**
 * str_contains_ctl_char tests whether s contains any ASCII control character
 */
bool str_contains_ctl_char(const char *s);

/**
 * ishex tests whether a character is valid hex
 */
bool ishex(char c);

/**
 * unhex converts the given char `c` into its hex integer equivalent
 * e.g. 'f' -> 15
 */
char unhex(char c);

/**
 * Convert a char `c` into a null-terminated char*
 * TODO: test
 */
char *tostr(char c);

/**
 * is_ascii tests whether a given character c is a printable ASCII character in
 * the range of 32 - 127
 */
bool is_ascii(char c);

/**
 * is_port_in_range tests whether the port is valid and non-reserved
 */
bool is_port_in_range(int port);
#endif /* UTIL_H */
