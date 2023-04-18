#ifndef HEADER_H
#define HEADER_H

#include "libhash/libhash.h"
#include "libutil/libutil.h"

extern const char CACHE_CONTROL[];
extern const char NO_CACHE[];
extern const char PRAGMA[];
extern const char CONTENT_TYPE[];
extern const char ACCEPT[];
extern const char USER_AGENT[];

/**
 * A 256 slot lookup table where each index corresponds to an ASCII character
 * and indicates whether that character is a valid header char
 *
 * @see https://httpwg.github.io/specs/rfc7230.html#rule.token.separators
 */
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
 * is_valid_header_field_char returns a bool indicating whether the given char
 * is a valid header char
 */
bool is_valid_header_field_char(unsigned int c);

/**
 * to_canonical_mime_header_key returns the canonical format of the MIME
 * header key `key`. The canonicalization converts the first letter and any
 * letter following a hyphen to upper case; the rest are converted to
 * lowercase.
 *
 * MIME header keys are assumed to be ASCII only.
 *
 * If `key` contains a space or invalid header field chars, it is returned
 * without modifications.
 */
char* to_canonical_mime_header_key(char* key);

// TODO: d + t
array_t* get_header_values(hash_table* headers, const char* key);

/**
 * get_first_header retrieves the first value for a given header key from
 * `headers`, or NULL if not found
 */
char* get_first_header(hash_table* headers, const char* key);

/**
 * req_header_values returns a char array of all values for the given header key
 * `key`. Returns NULL if the header does not exist
 *
 * TODO: public
 */
char** req_header_values(hash_table* headers, const char* key);

/**
 * insert_header inserts a key/value pair into the given hash table `headers`.
 * It accounts for singleton headers (headers that cannot be duplicated per HTTP
 * spec) and returns a bool indicating whether the header was successfully
 * inserted. If false, this means the header was a duplicate of an existing
 * singleton header.
 *
 * Can be used for both request and response headers, as they follow these same
 * rules.
 */
bool insert_header(hash_table* headers, const char* key, const char* value,
                   bool is_request);

/**
 * derive_headers extracts the comma-delimited headers in the value of the
 * given header
 */
array_t* derive_headers(const char* header_str);
#endif /* HEADER_H */
