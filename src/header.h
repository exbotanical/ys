#ifndef HEADER_H
#define HEADER_H

#include "libutil/libutil.h"  // for arrays

// A 256 slot lookup table where each index corresponds to an ASCII character
// and indicates whether that character is a valid header byte
// see: https://httpwg.github.io/specs/rfc7230.html#rule.token.separators
static const char token_table[] =
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\1\0\1\1\1\1\1\0\0\1\1\0\1\1\0\1"
    "\1\1\1\1\1\1\1\1\1\0\0\0\0\0\0\0\1\1\1\1\1\1\1\1"
    "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\0\0\1\1\1\1"
    "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1"
    "\1\0\1\0\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0";

/**
 * CanonicalMIMEHeaderKey returns the canonical format of the MIME header key s.
 * The canonicalization converts the first letter and any letter following a
 * hyphen to upper case; the rest are converted to lowercase.
 *
 * MIME header keys are assumed to be ASCII only.
 *
 * If s contains a space or invalid header field bytes, it is returned without
 * modifications.
 *
 * @param s
 * @return char*
 */
char* to_canonical_MIME_header_key(char* s);

/**
 * get_header_value retrieves the value for a given header key from `headers`,
 * or NULL if not found
 *
 * @param headers
 * @param key
 * @return char*
 */
char* get_header_value(array_t* headers, char* key);

/**
 * append_header appends the given key/value pair as a header object in
 * `headers`
 *
 * @param headers
 * @param key
 * @param value
 */
void append_header(array_t* headers, char* key, char* value);

#endif /* HEADER_H */
