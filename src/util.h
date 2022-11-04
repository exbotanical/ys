#ifndef UTIL_H
#define UTIL_H

#include "lib.hash/hash.h"

#include <stddef.h>

static const char NULL_TERM = '\0';

char *safe_itoa(int x);

char *fmt_str (char *fmt, ...);

#endif /* UTIL_H */
