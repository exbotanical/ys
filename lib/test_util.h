#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include "libhash/libhash.h"
#include "libutil/libutil.h"

typedef struct {
  char* key;
  array_t* values;
} header;

header* to_header(const char* key, array_t* values);

hash_table* to_headers(header* h, ...);

#endif /* TEST_UTIL_H */
