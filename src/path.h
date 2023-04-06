#ifndef PATH_H
#define PATH_H

#include <stdbool.h>

#include "libutil/libutil.h"

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

/**
 * path_split_first_delim splits the string p on the first PATH_DELIMITER
 * character and returns the resulting two strings
 *
 * @param p
 * @return array_t*
 */
array_t *path_split_first_delim(const char *p);

/**
 * path_split_dir splits the string p on the last PATH_DELIMITER
 * character and returns the resulting strings. If no PATH_DELIMITER is present,
 * the first element will be NULL and the second will be the full path
 *
 * @param p
 * @return array_t* [dir, filename]
 */
array_t *path_split_dir(const char *p);

/**
 * path_get_pure returns the "pure" path, or the path p sans any query
 * strings or fragments
 *
 * @param path
 * @return char*
 */
char *path_get_pure(const char *p);

#endif /* PATH_H */
