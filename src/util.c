#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

char *safe_itoa(int x) {
  int length = snprintf(NULL, 0, "%d", x);
  char *str = malloc(length + 1);
  snprintf(str, length + 1, "%d", x);

  return str;
}
