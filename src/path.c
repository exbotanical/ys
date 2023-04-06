#include "path.h"

#include <stdlib.h>
#include <string.h>

#include "libutil/libutil.h"
#include "logger.h"
#include "util.h"

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

array_t *path_split_first_delim(const char *p) {
  char *cp = s_copy(p);

  if (strlen(cp) <= 1) {
    return array_init();
  }

  // no slash at all
  char *a = strchr(cp, '/');
  if (!a) {
    return array_collect(fmt_str("/%s", cp), PATH_DELIMITER);
  }

  // slash is last char
  if (strlen(a) == 1) {
    cp[strlen(cp) - 1] = '\0';
    return array_collect(fmt_str("/%s", cp), PATH_DELIMITER);
  }

  char *s = strchr(cp + 1, '/');
  if (!s) {
    return array_collect(cp, PATH_DELIMITER);
  }

  char *f = malloc(strlen(cp) - strlen(s));
  strncpy(f, cp, (strlen(cp) - strlen(s)) + 1);
  f[strlen(cp) - strlen(s)] = '\0';

  return array_collect(f, s);
}

array_t *path_split_dir(const char *p) {
  char *cp = s_copy(p);
  array_t *paths = array_init();

  int i = strlen(cp);
  // TODO: handle windows volume if on windows
  while (i >= 0 && cp[i] != '/') {
    i--;
  }

  array_push(paths, s_substr(cp, 0, i + 1, false));
  array_push(paths, s_substr(cp, i + 1, strlen(cp), false));

  free(cp);
  return paths;
}

char *path_get_pure(const char *path) {
  char *cp = s_copy(path);

  char *q = strchr(cp, '?');
  if (!q) {
    return cp;
  }

  cp[strlen(cp) - strlen(q)] = '\0';

  return cp;
}
