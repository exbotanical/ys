#include "path.h"

#include <stdlib.h>  // for exit
#include <string.h>  // strlen, strtok, strstr

#include "logger.h"
#include "strdup/strdup.h"
#include "util.h"

const char *PATH_ROOT = "/";
const char *PATH_DELIMITER = "/";
const char *PARAMETER_DELIMITER = ":";
const char *PARAMETER_DELIMITER_START = "[";
const char *PARAMETER_DELIMITER_END = "]";
const char *PATTERN_WILDCARD = "(.+)";

array_t *expand_path(const char *path) { return split(path, PATH_DELIMITER); }

char *derive_label_pattern(const char *label) {
  int start = index_of(label, PARAMETER_DELIMITER_START);
  int end = index_of(label, PARAMETER_DELIMITER_END);

  // If the label doesn't contain a pattern, default to the wildcard pattern.
  if (start == -1 || end == -1) {
    return strdup(PATTERN_WILDCARD);
  }

  return substr(label, start + 1, end, false);
}

char *derive_parameter_key(const char *label) {
  int start = index_of(label, PARAMETER_DELIMITER);
  int end = index_of(label, PARAMETER_DELIMITER_START);

  if (end == -1) {
    end = strlen(label);
  }

  return substr(label, start + 1, end, false);
}
