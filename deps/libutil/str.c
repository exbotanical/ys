#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libutil.h"

size_t sizeof_char = sizeof(char);

char *str_truncate(const char *s, int n) {
  unsigned int full_len = strlen(s);
  unsigned int trunclen = abs(n);

  // Simply return a copy if invalid n
  if (n == 0 || trunclen >= full_len) {
    char *ret = malloc(full_len);
    strcpy(ret, s);

    return ret;
  }
  size_t sz = (size_t)full_len - trunclen;

  char *ret = malloc(sz + 1);
  if (!ret) {
    return NULL;
  }

  if (n > 0) {
    strncpy(ret, s + n, sz);
    ret[sz + 1] = '\0';
  } else {
    strncpy(ret, s, full_len - trunclen);
    ret[n] = '\0';
  }

  return ret;
}

char *str_concat(const char *s1, const char *s2) {
  unsigned int size = sizeof(s1) + sizeof(s1);
  char *ret = malloc(size);

  snprintf(ret, strlen(s1) + strlen(s2) + 1, "%s%s", s1, s2);

  return ret;
}
