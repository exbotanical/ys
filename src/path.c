#include "path.h"

#include <stdlib.h>  // for exit
#include <string.h>  // strlen, strtok, strstr

#include "libutil/libutil.h"  // for s_copy, s_indexof, s_substr
#include "logger.h"
#include "util.h"  // for split

const char PATH_ROOT[] = "/";
const char PATH_DELIMITER[] = "/";
const char PARAMETER_DELIMITER[] = ":";
const char PARAMETER_DELIMITER_START[] = "[";
const char PARAMETER_DELIMITER_END[] = "]";
const char PATTERN_WILDCARD[] = "(.+)";

array_t *expand_path(const char *path) { return split(path, PATH_DELIMITER); }

char *derive_label_pattern(const char *label) {
  int start = s_indexof(label, PARAMETER_DELIMITER_START);
  int end = s_indexof(label, PARAMETER_DELIMITER_END);

  // If the label doesn't contain a pattern, default to the wildcard pattern.
  if (start == -1 || end == -1) {
    return s_copy(PATTERN_WILDCARD);
  }

  return s_substr(label, start + 1, end, false);
}

char *derive_parameter_key(const char *label) {
  int start = s_indexof(label, PARAMETER_DELIMITER);
  int end = s_indexof(label, PARAMETER_DELIMITER_START);

  if (end == -1) {
    end = strlen(label);
  }

  return s_substr(label, start + 1, end, false);
}
