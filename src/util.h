#ifndef RESTC_UTIL_H
#define RESTC_UTIL_H

#include "lib.hash/hash.h" /* for TODO: */

#include <stddef.h>

static const char NULL_TERM = '\0';

char* safe_itoa(int x);

void iterate_h_table(h_table* ht, void (*cb)());

#endif /* RESTC_UTIL_H */
