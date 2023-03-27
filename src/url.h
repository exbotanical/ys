#ifndef URL_H
#define URL_H

#include <stdbool.h>

#include "libhash/libhash.h"

/**
 * parse_query parses the URL-encoded query string and returns a hash table
 * listing the values specified for each key. parse_query always returns a
 * non-NULL hash table containing all the valid query parameters found; if
 * errors are encountered, the key/value pair will be silently skipped.
 *
 * @param query query is expected to be a list of key=value settings separated
 * by ampersands. A setting without an equals sign is interpreted as a key set
 * to an empty value. Settings containing a non-URL-encoded semicolon are
 * considered invalid.
 * @return array_t*
 */
hash_table* parse_query(const char* query);

/**
 * has_query_string tests whether a string `url` has a valid query string
 *
 * @param url
 * @return true
 * @return false
 *
 * @see
 * https://stackoverflow.com/questions/1617058/ok-to-skip-slash-before-query-string
 */
bool has_query_string(const char* url);
#endif /* URL_H */
