#include "path.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

const char *PATH_ROOT = "/";
const char *PATH_DELIMITER = "/";
const char *PARAMETER_DELIMITER = ":";
const char *PARAMETER_DELIMITER_START = "[";
const char *PARAMETER_DELIMITER_END = "]";
const char *PATTERN_WILDCARD = "(.+)";

ch_array_t *expand_path(const char *path) {
  return split(path, PATH_DELIMITER);
}

ch_array_t *split(const char *str, const char *delimiter) {
  if (str == NULL || delimiter == NULL) {
    LOG("[path::split] invariant violation - null arguments(s), \
			where str was %s and delimiter was %s\n",
        str, delimiter);

    STDERR(
        "%s\n",
        "[path::split] invariant violation - null arguments(s) were provided");
    errno = EINVAL;

    return NULL;
  }

  // Maintain immutability
  // @see
  // https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
  char *input = strdup(str);

  ch_array_t *ca = ch_array_init();
  if (ca == NULL) {
    free(ca);

    LOG("[path::split] %s\n", "failed to allocate ch_array_t `ca`");

    return NULL;
  }

  // If the input doesn't even contain the delimiter, return early and avoid
  // further computation
  if (!strstr(input, delimiter)) {
    return ca;
  }

  // If the input *is* the delimiter, just return the empty array
  if (strcmp(input, delimiter) == 0) {
    return ca;
  }

  char *token = strtok(input, delimiter);
  if (token == NULL) {
    LOG(
        // TODO: Fix other log strings with tab chars being inadvertently
        // inserted
        "[path::split] `strtok` returned NULL for its initial token; this is likely a bug \
because the input contains the delimiter. input was %s and delimiter was %s\n",
        input, delimiter);

    return ca;
  }

  while (token != NULL) {
    ch_array_insert(ca, token);
    token = strtok(NULL, delimiter);
  }

  free(input);

  return ca;
}

int index_of(const char *str, const char *target) {
  if (str == NULL || target == NULL) {
    LOG("[path::index_of] invariant violation - null arguments(s), \
			where str was %s and target was %s\n",
        str, target);

    STDERR("%s\n",
           "[path::index_of] invariant violation - null arguments(s) were "
           "provided");
    errno = EINVAL;

    return -1;
  }

  char *needle = strstr(str, target);
  if (needle == NULL) {
    return -1;
  }

  return needle - str;
}

char *substr(const char *str, int start, int end, bool inclusive) {
  end = inclusive ? end : end - 1;

  if (start > end) {
    LOG("[path::substr] invariant violation - start index greater than end, \
			where str was %s, start was %d, end was %d, and inclusive flag was %s\n",
        str, start, end, inclusive ? "set" : "not set");

    STDERR("%s\n",
           "[path::substr] invariant violation - start index greater than end");
    errno = EINVAL;

    return NULL;
  }

  int len = strlen(str);
  if (start < 0 || start > len) {
    LOG("[path::substr] invariant violation - \
			start index less than zero or greater than str length, \
			where str was %s, start was %d, end was %d, and inclusive flag was %s\n",
        str, start, end, inclusive ? "set" : "not set");

    STDERR("%s\n",
           "[path::substr] invariant violation - \
			start index less than zero or greater than str length");
    errno = EINVAL;

    return NULL;
  }

  if (end > len) {
    LOG("[path::substr] invariant violation - end index was greater than str length, \
			where str was %s, start was %d, end was %d, and inclusive flag was %s\n",
        str, start, end, inclusive ? "set" : "not set");

    STDERR("%s\n",
           "[path::substr] invariant violation - end index was greater than "
           "str length");
    errno = EINVAL;

    return NULL;
  }

  int size_multiplier = end - start;
  char *ret = malloc(sizeof(char) * size_multiplier);
  if (ret == NULL) {
    free(ret);

    LOG("[path::substr] failed to allocate char* with `malloc`, where \
			size multiplier was %d\n",
        size_multiplier);

    STDERR("%s\n", "[path::substr] failed to allocate char* with `malloc`");

    return NULL;
  }

  int i = 0;
  int j = 0;
  for (i = start, j = 0; i <= end; i++, j++) {
    ret[j] = str[i];
  }

  ret[j] = '\0';

  return ret;
}

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
