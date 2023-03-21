#ifndef LIBUTIL_H
#define LIBUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#define foreach(arr, i) for (unsigned int i = 0; i < array_size(arr); i++)
#define foreach_i(arr, i, start) \
  for (unsigned int i = start; i < array_size(arr); i++)

#define has_elements(arr) (arr != NULL && array_size(arr) > 0)

#define array_collect(...) __array_collect(__VA_ARGS__, NULL)

typedef struct {
  void **state;
  unsigned int len;
} __array_t;

typedef __array_t *array_t;

typedef void *callback_function_t(void *el, unsigned int index, array_t *array);
typedef bool predicate_function_t(void *el, unsigned int index, array_t *array,
                                  void *compare_to);
typedef bool comparator_function_t(void *el, void *compare_to);

unsigned int array_size(array_t *array);

void *array_get(array_t *array, int index);

array_t *array_init();

bool array_includes(array_t *array, comparator_function_t *comparator,
                    void *compare_to);

int array_find(array_t *array, comparator_function_t *comparator,
               void *compare_to);

bool array_push(array_t *array, void *el);

void *array_pop(array_t *array);

void *array_shift(array_t *array);

array_t *array_slice(array_t *array, unsigned int start, int end);

bool array_remove(array_t *array, unsigned int idx);

array_t *array_map(array_t *array, callback_function_t *callback);

array_t *array_filter(array_t *array, predicate_function_t *predicate,
                      void *compare_to);

void array_foreach(array_t *array, callback_function_t *callback);

void array_free(array_t *array);

unsigned int array_size(array_t *array);

array_t *__array_collect(void *v, ...);

/**
 * @brief A dynamic string buffer.
 */
typedef struct {
  char *state;
  unsigned int len;
} __buffer_t;

typedef __buffer_t *buffer_t;

int buffer_size(buffer_t *buf);

char *buffer_state(buffer_t *buf);

/**
 * @brief Allocates memory for a new buffer.
 *
 * @return buffer_t*
 */
buffer_t *buffer_init(const char *init);

/**
 * @brief Appends a string to a given buffer `buf`, reallocating the required
 * memory
 *
 * @param buf the buffer to which `s` will be appended
 * @param s char pointer to be appended to the buffer
 */
bool buffer_append(buffer_t *buf, const char *s);

/**
 * @brief Appends a string to a given buffer `buf`, reallocating the required
 * memory
 *
 * @param buf the buffer to which `s` will be appended
 * @param s char pointer to be appended to the buffer
 * @param len the length at which to truncate the given string `s`
 */
bool buffer_append_with(buffer_t *buf, const char *s, unsigned int len);

buffer_t *buffer_concat(buffer_t *buf_a, buffer_t *buf_b);

/**
 * @brief Deallocate the dynamic memory used by an `buffer_t`
 *
 * @param buf the buffer pointer
 */
void buffer_free(buffer_t *buf);

char *fmt_str(char *fmt, ...);

char *str_truncate(const char *s, int n);

char *str_concat(const char *s1, const char *s2);

#ifdef __cplusplus
}
#endif

#endif /* LIBUTIL_H */
