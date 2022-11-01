#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

typedef struct array {
  int* array;
  size_t used;
  size_t size;
} array_t;

#endif /* ARRAY_H */
