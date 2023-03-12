#include "util.h"

#include <stdio.h>   // for snprintf
#include <stdlib.h>  // for malloc

char *safe_itoa(int x) {
  int length = snprintf(NULL, 0, "%d", x);
  char *str = malloc(length + 1);
  snprintf(str, length + 1, "%d", x);

  return str;
}
