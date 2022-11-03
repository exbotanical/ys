#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

typedef struct ch_array {
  char** state;
  size_t size;
} ch_array_t;

ch_array_t *ch_array_init();

void ch_array_insert(ch_array_t *a, char *el);

void ch_array_free(ch_array_t *a);

typedef struct array {
  void** state;
  size_t size;
} array_t;

array_t *array_init();

void array_insert(array_t *a, void *el);

void array_free(array_t *a);

#endif /* ARRAY_H */
