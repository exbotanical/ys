#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief TODO: doc
 *
 */
typedef struct ch_array {
  char** state;
  size_t size;
} ch_array_t;

/**
 * @brief  TODO: doc
 *
 */
typedef struct array {
  void** state;
  size_t size;
} array_t;

/**
 * @brief Allocates memory for a new character array and its `state` member.
 *
 * @return ch_array_t* Initialized character array
 */
ch_array_t *ch_array_init();

/**
 * @brief Inserts a char pointer element into the given character array `a`.
 *
 * @param a The character array into which `el` will be inserted
 * @param el The char pointer element to insert
 *
 * @return A boolean indicating whether the insertion succeeded
 */
bool ch_array_insert(ch_array_t *a, char *el);

/**
 * @brief Deallocates memory for character array `a`.
 *
 * @param a The array to deallocate
 */
void ch_array_free(ch_array_t *a);

/**
 * @brief Allocates memory for a new void pointer array and its `state` member.
 *
 * @return array_t* Initialized void pointer array
 */
array_t *array_init();

/**
 * @brief Inserts a void pointer element into the given void pointer array `a`.
 *
 * @param a The void pointer array into which `el` will be inserted
 * @param el The void pointer element to insert
 *
 * @return A boolean indicating whether the insertion succeeded
 */
bool array_insert(array_t *a, void *el);

/**
 * @brief Deallocates memory for array `a`.
 *
 * @param a The array to deallocate
 */
void array_free(array_t *a);

#endif /* ARRAY_H */
