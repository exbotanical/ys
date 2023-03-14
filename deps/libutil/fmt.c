#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "libutil.h"

char *fmt_str(char *fmt, ...) {
  va_list args, args_cp;
  va_start(args, fmt);
  va_copy(args_cp, args);

  // Pass length of zero first to determine number of bytes needed
  unsigned int n = vsnprintf(NULL, 0, fmt, args) + 1;
  char *buf = malloc(n);
  if (!buf) {
    return NULL;
  }

  vsnprintf(buf, n, fmt, args_cp);

  va_end(args);
  va_end(args_cp);

  return buf;
}
