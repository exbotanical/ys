#include <ctype.h>  // for toupper
#include <stdio.h>  // for snprintf
#include <stdlib.h>
#include <string.h>
#include <strings.h>  // for strncasecmp

#include "libutil.h"

static bool is_ascii_space(char b) {
  return b == ' ' || b == '\t' || b == '\n' || b == '\r';
}

char *s_truncate(const char *s, int n) {
  unsigned int full_len = strlen(s);
  unsigned int trunclen = abs(n);

  // Simply return a copy if invalid n
  if (n == 0 || trunclen >= full_len) {
    return s_copy(s);
  }

  size_t sz = (size_t)full_len - trunclen;

  char *ret = malloc(sz + 1);
  if (!ret) {
    return NULL;
  }

  if (n > 0) {
    sprintf(ret, s + n);
  } else {
    snprintf(ret, full_len - trunclen + 1, s);
  }

  return ret;
}

char *s_concat(const char *s1, const char *s2) {
  unsigned int size = sizeof(s1) + sizeof(s1);
  char *ret = malloc(size);
  if (!ret) {
    return NULL;
  }

  snprintf(ret, strlen(s1) + strlen(s2) + 1, "%s%s", s1, s2);

  return ret;
}

char *s_copy(const char *s) {
  if (NULL == (char *)s) {
    return NULL;
  }

  int len = strlen(s) + 1;
  char *buf = malloc(len);

  if (buf) {
    memset(buf, 0, len);
    memcpy(buf, s, len - 1);
  }
  return buf;
}

int s_indexof(const char *s, const char *target) {
  if (s == NULL || target == NULL) {
    return -1;
  }

  char *needle = strstr(s, target);
  if (needle == NULL) {
    return -1;
  }

  return needle - s;
}

char *s_substr(const char *s, int start, int end, bool inclusive) {
  end = inclusive ? end : end - 1;

  if (start > end) {
    return NULL;
  }

  int len = strlen(s);
  if (start < 0 || start > len) {
    return NULL;
  }

  if (end > len) {
    return NULL;
  }

  int size_multiplier = end - start;
  // Fix: Fatal glibc error: malloc.c:2593 (sysmalloc): assertion failed:
  // (old_top == initial_top (av) && old_size == 0) || ((unsigned long)
  // (old_size) >= MINSIZE && prev_inuse (old_top) && ((unsigned long) old_end &
  // (pagesize - 1)) == 0)
  char *ret = malloc(sizeof(char) * size_multiplier + 2);
  if (!ret) {
    return NULL;
  }

  int i = 0;
  int j = 0;
  for (i = start, j = 0; i <= end; i++, j++) {
    ret[j] = s[i];
  }

  ret[j] = '\0';

  return ret;
}

bool s_casecmp(const char *s1, const char *s2) {
  unsigned int s1l = strlen(s1);
  unsigned int s2l = strlen(s2);

  unsigned int compare_chars = s1l > s2l ? s1l : s2l;
  return strncasecmp(s1, s2, compare_chars) == 0;
}

char *s_upper(const char *s) {
  size_t l = strlen(s);

  char *ca = malloc(l + 1);
  if (!ca) {
    return NULL;
  }

  strncpy(ca, s, l);
  ca[l] = '\0';

  for (size_t i = 0; i < l; i++) {
    ca[i] = toupper(s[i]);
  }

  return ca;
}

bool s_equals(const char *s1, const char *s2) {
  if (!s1 && !s2)
    return true;
  else if (!s1)
    return false;
  else if (!s2)
    return false;
  else
    return strcmp(s1, s2) == 0;
}

bool s_nullish(const char *s) { return s == NULL || s_equals(s, ""); }

char *s_trim(const char *s) {
  char *scp = s_copy(s);

  while (strlen(scp) > 0 && is_ascii_space(scp[0])) {
    char *nscp = s_substr(scp, 1, strlen(scp), true);
    free(scp);
    scp = nscp;
  }

  while (strlen(scp) > 0 && is_ascii_space(scp[strlen(scp) - 1])) {
    char *nscp = s_substr(scp, 0, strlen(scp) - 1, false);
    free(scp);
    scp = nscp;
  }

  return scp;
}

array_t *s_split(const char *s, const char *delim) {
  if (s == NULL || delim == NULL) {
    return NULL;
  }

  // see:
  // https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
  char *input = s_copy(s);

  array_t *tokens = array_init();
  if (tokens == NULL) {
    free(input);
    return NULL;
  }

  // If the input doesn't even contain the delimiter, return early and avoid
  // further computation
  if (!strstr(input, delim)) {
    free(input);
    return tokens;
  }

  // If the input *is* the delimiter, just return the empty array
  if (s_equals(input, delim)) {
    free(input);
    return tokens;
  }

  char *token = strtok(input, delim);
  if (token == NULL) {
    free(input);
    return tokens;
  }

  while (token != NULL) {
    array_push(tokens, s_copy(token));
    token = strtok(NULL, delim);
  }

  free(input);

  return tokens;
}
