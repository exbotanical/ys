#ifndef PATH_H
#define PATH_H

#include "array.h"

#include <stdbool.h>

static const char* PATH_ROOT = "/";
static const char* PATH_DELIMITER = "/";
static const char* PARAMETER_DELIMITER = ":";
static const char* PARAMETER_DELIMITER_START = "[";
static const char* PARAMETER_DELIMITER_END = "]";
static const char* PATTERN_WILDCARD = "(.+)";

// static h_table* regex_cache = h_init_table(0);

ch_array_t *split(const char* str, const char* delimiter);

int index_of (const char* str, const char* target);

char* substr (const char* str, int start, int end, bool inclusive);

ch_array_t *expand_path(const char* path);

char* derive_label_pattern(const char* label);
char* derive_parameter_key(const char* label);

char* regex_cache_get(char* pattern);

#endif /* PATH_H */
