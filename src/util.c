#include "util.h"

#include <ctype.h>   // for toupper
#include <stdarg.h>  // for variadic args functions
#include <stdio.h>   // for snprintf
#include <string.h>  // for strtok, strstr

#include "libutil/libutil.h"  // for s_copy, s_equals
#include "logger.h"           // for DIE
#include "xmalloc.h"

// TODO: fix
char *safe_itoa(int x) {
  int length = snprintf(NULL, 0, "%d", x);
  char *str = xmalloc(length + 1);
  snprintf(str, length + 1, "%d", x);

  return str;
}

array_t *split(const char *s, const char *delim) {
  if (s == NULL || delim == NULL) {
    printlogf(
        LOG_INFO,
        "[path::%s] invariant violation - null arguments(s), where string "
        "was %s and delimiter was %s\n",
        __func__, s, delim);

    return NULL;
  }

  // see:
  // https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
  char *input = s_copy(s);

  array_t *tokens = array_init();
  if (tokens == NULL) {
    free(tokens);
    DIE(EXIT_FAILURE, "[path::%s] failed to allocate array_t `tokens`\n",
        __func__);
  }

  // If the input doesn't even contain the delimiter, return early and avoid
  // further computation
  if (!strstr(input, delim)) {
    return tokens;
  }

  // If the input *is* the delimiter, just return the empty array
  if (s_equals(input, delim)) {
    return tokens;
  }

  char *token = strtok(input, delim);
  if (token == NULL) {
    printlogf(LOG_INFO,
              "[path::%s] `strtok` returned NULL for its initial token; "
              "this is likely a bug because the input contains the delimiter. "
              "input was %s and delimiter was %s\n",
              __func__, input, delim);

    return tokens;
  }

  while (token != NULL) {
    array_push(tokens, s_copy(token));
    token = strtok(NULL, delim);
  }

  free(input);

  return tokens;
}

char *str_join(array_t *sarr, const char *delim) {
  buffer_t *buf = buffer_init(NULL);

  unsigned int sz = array_size(sarr);

  for (unsigned int i = 0; i < sz; i++) {
    const char *s = array_get(sarr, i);
    buffer_append(buf, s);

    if (i != sz - 1) {
      buffer_append(buf, delim);
    }
  }

  return buffer_state(buf);
}

array_t *str_cut(const char *s, const char *sep) {
  int i = s_indexof(s, sep);

  if (i >= 0) {
    char *first = s_substr(s, 0, i, false);
    char *second = s_substr(s, i + strlen(sep), strlen(s), true);

    return array_collect(s_nullish(first) ? NULL : first,
                         s_nullish(second) ? NULL : second);
  }

  return array_collect(s_copy(s));
}

bool str_contains_ctl_char(const char *s) {
  for (unsigned int i = 0; i < strlen(s); i++) {
    char b = s[i];
    if (b < ' ' || b == 0x7f) {
      return true;
    }
  }
  return false;
}

bool ishex(char c) {
  return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') ||
         ('A' <= c && c <= 'F');
}

char unhex(char c) {
  if ('0' <= c && c <= '9') return c - '0';
  if ('a' <= c && c <= 'f') return c - 'a' + 10;
  if ('A' <= c && c <= 'F') return c - 'A' + 10;

  return 0;
}

char *tostr(char c) {
  char *strp = malloc(2);
  char str[2] = {c, '\0'};
  memcpy(strp, str, 2);
  return strp;
}

bool is_ascii(char c) { return 0x20 <= c && c < 0x7f; }
