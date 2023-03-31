#ifndef PATH_H
#define PATH_H

#include <stdbool.h>  // For boolean aliases

#include "libutil/libutil.h"  // For array_t

// Default root path e.g. /
extern const char PATH_ROOT[];

// Default path delimiter e.g. /
extern const char PATH_DELIMITER[];

// Default parameter delimiter e.g. :
extern const char PARAMETER_DELIMITER[];

// Default parameter start delimiter e.g. [
extern const char PARAMETER_DELIMITER_START[];

// Default parameter end delimiter e.g. ]
extern const char PARAMETER_DELIMITER_END[];

// Default pattern wildcard e.g. (.+)
extern const char PATTERN_WILDCARD[];

/**
 * Expands a path into an array of all words delimited by
 * `PATH_DELIMITER`. Sets errno if provided null or otherwise invalid
 * argument(s).
 *
 * @param path The path to expand
 * @return array_t*
 */
array_t *expand_path(const char *path);

/**
 * Extracts from a label a PCRE-compliant regular expression. Sets errno
 * if provided null or otherwise invalid argument(s).
 *
 * @param label The string label that should contain a PCRE-compliant regex
 * @return char* The regex pattern, or a wildcard if none found
 */
char *derive_label_pattern(const char *label);

/**
 * Extracts from a label a parameter key. Sets errno if provided null or
 * otherwise invalid argument(s).
 *
 * @param label The string label that should contain a parameter key/value pair
 * @return char* The matching parameter key
 */
char *derive_parameter_key(const char *label);

#endif /* PATH_H */
