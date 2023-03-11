#ifndef PATH_H
#define PATH_H

#include <stdbool.h>  // For boolean aliases

#include "libutil/libutil.h"  // For array_t

/**
 * @brief Default root path e.g. /
 */
extern const char *PATH_ROOT;

/**
 * @brief Default path delimiter e.g. /
 */
extern const char *PATH_DELIMITER;

/**
 * @brief Default parameter delimiter e.g. :
 */
extern const char *PARAMETER_DELIMITER;

/**
 * @brief Default parameter start delimiter e.g. [
 */
extern const char *PARAMETER_DELIMITER_START;

/**
 * @brief Default parameter end delimiter e.g. ]
 */
extern const char *PARAMETER_DELIMITER_END;

/**
 * @brief Default pattern wildcard e.g. (.+)
 */
extern const char *PATTERN_WILDCARD;

/**
 * @brief Expands a path into an array of all words delimited by
 * `PATH_DELIMITER`. Sets errno if provided null or otherwise invalid
 * argument(s).
 *
 * @param path The path to expand
 * @return array_t*
 */
array_t *expand_path(const char *path);

/**
 * @brief Splits a string on all instances of a delimiter. Sets errno if
 * provided null or otherwise invalid argument(s).
 *
 * @param str The string to split
 * @param delimiter The delimiter on which to split `str`
 * @return array_t* An array of matches, if any; NULL if erroneous
 */
array_t *split(const char *str, const char *delimiter);

/**
 * @brief Returns the index of a character `target`
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
 * @brief For a given string `str`, finds and returns the substring between
 * indices `start` and `end`. Sets errno if provided null or otherwise invalid
 * argument(s).
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
 * @brief Extracts from a label a PCRE-compliant regular expression. Sets errno
 * if provided null or otherwise invalid argument(s).
 *
 * @param label The string label that should contain a PCRE-compliant regex
 * @return char* The regex pattern, or a wildcard if none found
 */
char *derive_label_pattern(const char *label);

/**
 * @brief Extracts from a label a parameter key. Sets errno if provided null or
 * otherwise invalid argument(s).
 *
 * @param label The string label that should contain a parameter key/value pair
 * @return char* The matching parameter key
 */
char *derive_parameter_key(const char *label);

#endif /* PATH_H */
