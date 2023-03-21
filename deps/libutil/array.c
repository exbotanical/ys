#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>

#include "libutil.h"

unsigned int array_size(array_t *array) { return ((__array_t *)array)->len; }

void *array_get(array_t *array, int index) {
  __array_t *internal = (__array_t *)array;
  if (internal->len == 0) {
    return NULL;
  }

  unsigned int absolute = abs(index);
  if (absolute >= internal->len) {
    return NULL;
  }

  if (index < 0) {
    return internal->state[internal->len - absolute];
  }

  return internal->state[index];
}

array_t *array_init() {
  array_t *array = malloc(sizeof(array_t));
  if (!array) {
    errno = ENOMEM;
    return NULL;
  }

  __array_t *internal = (__array_t *)array;

  internal->state = malloc(sizeof(void *));
  if (!internal->state) {
    errno = ENOMEM;
    return NULL;
  }

  internal->len = 0;

  return array;
}

array_t *__array_collect(void *v, ...) {
  array_t *arr = array_init();

  va_list args;
  va_start(args, v);

  array_push(arr, v);
  while ((v = va_arg(args, void *))) {
    array_push(arr, v);
  }

  va_end(args);

  return arr;
}

bool array_includes(array_t *array, comparator_function_t *comparator,
                    void *compare_to) {
  __array_t *internal = (__array_t *)array;

  for (unsigned int i = 0; i < internal->len; i++) {
    if (comparator(internal->state[i], compare_to)) {
      return true;
    }
  }

  return false;
}

int array_find(array_t *array, comparator_function_t *comparator,
               void *compare_to) {
  __array_t *internal = (__array_t *)array;

  for (unsigned int i = 0; i < internal->len; i++) {
    if (comparator(internal->state[i], compare_to)) {
      return i;
    }
  }

  return -1;
}

bool array_push(array_t *array, void *el) {
  __array_t *internal = (__array_t *)array;

  void **next_state =
      realloc(internal->state, (internal->len + 1) * sizeof(void *));
  if (!next_state) {
    free(next_state);
    errno = ENOMEM;

    return false;
  }

  internal->state = next_state;
  internal->state[internal->len++] = el;

  return true;
}

void *array_pop(array_t *array) {
  __array_t *internal = (__array_t *)array;

  unsigned int len = internal->len;

  if (len > 0) {
    unsigned int next_len = len - 1;

    void *el = internal->state[next_len];
    internal->state[next_len] = NULL;
    internal->len = next_len;

    return el;
  }

  return NULL;
}

void *array_shift(array_t *array) {
  __array_t *internal = (__array_t *)array;

  if (internal->len == 0) {
    return false;
  }

  void *el = internal->state[0];
  array_t *new = array_init();

  for (unsigned int i = 1; i < internal->len; i++) {
    array_push(new, internal->state[i]);
  }

  internal->len--;
  *array = *new;
  return el;
}

array_t *array_slice(array_t *array, unsigned start, int end) {
  __array_t *internal = (__array_t *)array;
  array_t *slice = array_init();

  // TODO: test -1
  unsigned int normalized_end = end == -1 ? (int)internal->len : end;
  for (unsigned int i = start; i < normalized_end; i++) {
    array_push(slice, internal->state[i]);
  }

  return slice;
}

bool array_remove(array_t *array, unsigned int index) {
  __array_t *internal = (__array_t *)array;

  if (internal->len < index) {
    return false;
  }

  for (unsigned int i = 0; i < internal->len - 1; i++) {
    if (i >= index) {
      internal->state[i] = internal->state[i + 1];
    }
  }

  internal->len--;
  return true;
}

array_t *array_map(array_t *array, callback_function_t *callback) {
  __array_t *internal = (__array_t *)array;

  array_t *ret = array_init();
  for (unsigned int i = 0; i < internal->len; i++) {
    array_push(ret, callback(internal->state[i], i, array));
  }

  return ret;
}

array_t *array_filter(array_t *array, predicate_function_t *predicate,
                      void *compare_to) {
  __array_t *internal = (__array_t *)array;

  array_t *ret = array_init();
  for (unsigned int i = 0; i < internal->len; i++) {
    void *el = internal->state[i];
    if (predicate(el, i, array, compare_to)) {
      array_push(ret, el);
    }
  }

  return ret;
}

void array_foreach(array_t *array, callback_function_t *callback) {
  __array_t *internal = (__array_t *)array;

  for (unsigned int i = 0; i < internal->len; i++) {
    callback(internal->state[i], i, array);
  }
}

void array_free(array_t *array) {
  __array_t *internal = (__array_t *)array;

  free(internal->state);
  internal->state = NULL;
  free(internal);
}
