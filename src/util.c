#include "util.h"

#include <ctype.h>   // for toupper
#include <stdarg.h>  // for variadic args functions
#include <stdio.h>   // for snprintf
#include <string.h>  // for strtok, strstr

#include "libutil/libutil.h"  // for s_copy, s_equals
#include "logger.h"           // for DIE
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
  char *input = s_copy(str);

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
  if (s_equals(input, delimiter)) {
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
    array_push(tokens, s_copy(token));
    token = strtok(NULL, delimiter);
  }

  free(input);

  return tokens;
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
