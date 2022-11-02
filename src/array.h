#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

typedef struct ch_array {
  char** state;
  size_t size;
} ch_array_t;

ch_array_t *ch_array_init(size_t initial_size);

void ch_array_insert(ch_array_t *a, char* el);

void ch_array_free(ch_array_t *a);

#endif /* ARRAY_H */
