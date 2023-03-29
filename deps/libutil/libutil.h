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

/**
 * s_truncate truncates the given string `s` by `n` characters
 *
 * @param s
 * @param n
 * @return char*
 */
char *s_truncate(const char *s, int n);

/**
 * s_concat concatenates two strings `s1` and `s2` in that order
 *
 * @param s1
 * @param s2
 * @return char*
 */
char *s_concat(const char *s1, const char *s2);

/**
 * s_copy returns a copy of given string `str`. Compare to strdup
 *
 * @param s
 * @return char*
 */
char *s_copy(const char *s);

/**
 * s_indexof returns the index of a character `target`
 * as it exists in a character array `str`.
 *
 * @param str The character array, or "haystack"
 * @param target The target, or "needle"
 *
 * @return int The index of `target` qua `str`, or -1 if not extant
 */
int s_indexof(const char *str, const char *target);

/**
 * s_substr finds and returns the substring between
 * indices `start` and `end` for a given string `str`.
 *
 * @param s The string in which to find a substring
 * @param start The starting index, always inclusive
 * @param end The ending index, only inclusive if specified
 * @param inclusive A flag indicating whether the substring match should be
 * end-inclusive
 * @return char* The matching substring, or NULL if erroneous
 */
char *s_substr(const char *s, int start, int end, bool inclusive);

/**
 * s_casecmp performs a safe case-insensitive string comparison between
 * two strings and returns a bool indicating whether they are equal
 *
 * @param s1
 * @param s2
 * @return bool
 */
bool s_casecmp(const char *s1, const char *s2);

/**
 * s_upper converts the given string `s` to uppercase
 *
 * @param s
 * @return char*
 */
char *s_upper(const char *s);

/**
 * s_equals returns a bool indicating whether strings s1 and s2 are completely
 * equal (case-sensitive)
 *
 * @param s1
 * @param s2
 * @return bool
 */
bool s_equals(const char *s1, const char *s2);

/**
 * s_nullish tests whether a given string `s` is NULL or empty i.e. ""
 *
 * @param s
 * @return true
 * @return false
 */
bool s_nullish(const char *s);

char *s_trim(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* LIBUTIL_H */
