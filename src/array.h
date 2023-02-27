#ifndef CH_ARRAY_H
#define CH_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief A dynamic character pointer array.
 */
typedef struct ch_array {
  char **state;
  size_t size;
} ch_array_t;

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

#endif /* CH_ARRAY_H */
