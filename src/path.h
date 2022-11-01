#ifndef PATH_H
#define PATH_H

#include "buffer.h"

static const char* PATH_ROOT = "/";
static const char* PATH_DELIMITER = "/";
static const char* PARAMETER_DELIMITER = ":";
static const char* PARAMETER_DELIMITER_START = "[";
static const char* PARAMETER_DELIMITER_END = "]";
static const char* PATTERN_WILDCARD = "(.+)";

// static h_table* regex_cache = h_init_table(0);

buffer_t* split(const char* str, const char* delimiter);

char* expand_path(const char* path);
char* derive_label_pattern(const char* label);
char* derive_parameter_key(const char* label);

char* regex_cache_get(char* pattern);

#endif /* PATH_H */
