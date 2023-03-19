#ifndef HEADER_H
#define HEADER_H

#include "libhash/libhash.h"  // for hash sets and hash tables
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
 * req_header_get retrieves the first value for a given header key from
 * `headers`, or NULL if not found
 * TODO: pub
 *
 * @param headers
 * @param key
 * @return char*
 */
char* req_header_get(hash_table* headers, const char* key);

/**
 * req_header_values returns a char array of all values for the given header key
 * `key`. Returns NULL if the header does not exist
 *
 * @param headers
 * @param key
 * @return char**
 */
char** req_header_values(hash_table* headers, const char* key);

/**
 * res_header_append appends the given key/value pair as a header object in
 * `headers`
 *
 * @param headers
 * @param key
 * @param value
 */
void res_header_append(array_t* headers, const char* key, const char* value);

/**
 * insert_header inserts a key/value pair into the given hash table `headers`.
 * It accounts for singleton headers (headers that cannot be duplicated per HTTP
 * spec) and returns a bool indicating whether the header was successfully
 * inserted. If false, this means the header was a duplicate of an existing
 * singleton header.
 *
 * Can be used for both request and response headers, as they follow these same
 * rules.
 *
 * @param headers
 * @param k
 * @param v
 * @return bool
 */
bool insert_header(hash_table* headers, const char* k, const char* v);
#endif /* HEADER_H */
