#include "util.h"

#include <ctype.h>   // for toupper
#include <stdarg.h>  // for variadic args functions
#include <stdio.h>   // for snprintf
#include <string.h>  // for strcmp, strncpy, strlen, strncasecmp

#include "logger.h"  // for DIE
#include "strdup/strdup.h"
#include "xmalloc.h"

char *safe_itoa(int x) {
  int length = snprintf(NULL, 0, "%d", x);
  char *str = xmalloc(length + 1);
  snprintf(str, length + 1, "%d", x);

  return str;
}

array_t *split(const char *str, const char *delimiter) {
  if (str == NULL || delimiter == NULL) {
    printlogf(LOG_INFO,
              "[path::%s] invariant violation - null arguments(s), where str "
              "was %s and delimiter was %s\n",
              __func__, str, delimiter);

    return NULL;
  }

  // Maintain immutability
  // @see
  // https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
  char *input = strdup(str);

  array_t *tokens = array_init();
  if (tokens == NULL) {
    free(tokens);
    DIE(EXIT_FAILURE, "[path::%s] failed to allocate array_t `tokens`\n",
        __func__);
  }

  // If the input doesn't even contain the delimiter, return early and avoid
  // further computation
  if (!strstr(input, delimiter)) {
    return tokens;
  }

  // If the input *is* the delimiter, just return the empty array
  if (str_equals(input, delimiter)) {
    return tokens;
  }

  char *token = strtok(input, delimiter);
  if (token == NULL) {
    printlogf(LOG_INFO,
              "[path::%s] `strtok` returned NULL for its initial token; "
              "this is likely a bug because the input contains the delimiter. "
              "input was %s and delimiter was %s\n",
              __func__, input, delimiter);

    return tokens;
  }

  while (token != NULL) {
    array_push(tokens, strdup(token));
    token = strtok(NULL, delimiter);
  }

  free(input);

  return tokens;
}

int index_of(const char *str, const char *target) {
  if (str == NULL || target == NULL) {
    printlogf(LOG_INFO,
              "[path::%s] invariant violation - null arguments(s), where str "
              "was %s and target was %s\n",
              __func__, str, target);

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
    printlogf(
        LOG_INFO,
        "[path::%s] invariant violation - start index greater than end, where "
        "str was %s, start was %d, end was %d, and inclusive flag was %s\n",
        __func__, str, start, end, inclusive ? "set" : "not set");

    return NULL;
  }

  int len = strlen(str);
  if (start < 0 || start > len) {
    printlogf(LOG_INFO,
              "[path::%s] invariant violation - start index less than zero or "
              "greater than str length, where str was %s, start was %d, end "
              "was %d, and inclusive flag was %s\n",
              __func__, str, start, end, inclusive ? "set" : "not set");

    return NULL;
  }

  if (end > len) {
    printlogf(LOG_INFO,
              "[path::%s] invariant violation - end index was greater than str "
              "length, where str was %s, start was %d, end was %d, and "
              "inclusive flag was %s\n",
              __func__, str, start, end, inclusive ? "set" : "not set");

    return NULL;
  }

  int size_multiplier = end - start;
  char *ret = xmalloc(sizeof(char) * size_multiplier);

  int i = 0;
  int j = 0;
  for (i = start, j = 0; i <= end; i++, j++) {
    ret[j] = str[i];
  }

  ret[j] = '\0';

  return ret;
}

bool safe_strcasecmp(const char *s1, const char *s2) {
  unsigned int s1l = strlen(s1);
  unsigned int s2l = strlen(s2);

  unsigned int compare_chars = s1l > s2l ? s1l : s2l;
  return strncasecmp(s1, s2, compare_chars) == 0;
}

char *str_join(array_t *strarr, const char *delim) {
  buffer_t *buf = buffer_init(NULL);

  unsigned int sz = array_size(strarr);

  for (unsigned int i = 0; i < sz; i++) {
    const char *str = array_get(strarr, i);
    buffer_append(buf, str);

    if (i != sz - 1) {
      buffer_append(buf, delim);
    }
  }

  return buffer_state(buf);
}

char *to_upper(const char *s) {
  size_t l = strlen(s);

  char *ca = xmalloc(l + 1);
  strncpy(ca, s, l);
  ca[l] = '\0';

  for (size_t i = 0; i < l; i++) {
    ca[i] = toupper(s[i]);
  }

  return ca;
}

bool str_equals(const char *s1, const char *s2) { return strcmp(s1, s2) == 0; }

bool str_nullish(const char *s) { return s == NULL || str_equals(s, ""); }
