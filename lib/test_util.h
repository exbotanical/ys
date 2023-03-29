#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include "libhash/libhash.h"
#include "libutil/libutil.h"

typedef struct {
  char* key;
  array_t* values;
} header;

header* to_header(const char* key, array_t* values) {
  header* h = malloc(sizeof(header));
  h->key = key;
  h->values = values;

  return h;
}

hash_table* to_headers(header* h, ...) {
  hash_table* headers = ht_init(0);

  va_list args;
  va_start(args, h);

  while (h) {
    ht_insert(headers, h->key, h->values);
    h = va_arg(args, header*);
  }

  va_end(args);

  return headers;
}

header* to_header(const char* key, array_t* values);

hash_table* to_headers(header* h, ...);

#endif /* TEST_UTIL_H */
