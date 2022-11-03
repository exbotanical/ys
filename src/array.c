#include "array.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocates memory for a new character array and its `state` member
 *
 * @return ch_array_t* Initialized character array
 */
ch_array_t *ch_array_init() {
	ch_array_t *a = malloc(sizeof(ch_array_t));
	if (a == NULL) {
		LOG("[array::ch_array_init] failed to allocate ch_array_t\n");
		return NULL;
	}

  a->state = malloc(sizeof(char*));
	if (a->state == NULL) {
		LOG("[array::ch_array_init] failed to allocate ch_array_t::state\n");
		return NULL;
	}

  a->size = 0;

	return a;
}

/**
 * @brief Inserts a char pointer element into the given character array `a`
 *
 * @param a The character array into which `el` will be inserted
 * @param el The char pointer element to insert
 *
 * @return A boolean indicating whether the insertion succeeded
 */
bool ch_array_insert(ch_array_t *a, char *el) {
	char *cp = strdup(el);
	if (cp == NULL) {
		LOG(
			"[array::ch_array_insert] failed to copy provided element with `strdup`, \
			where `a` was %p and `el` was %s\n",
			a,
			el
		);

		return false;
	}

	char **next_state = realloc(a->state, (a->size + 1) * sizeof(char*));
	if (next_state == NULL) {
		LOG(
			"[array::ch_array_insert] failed to reallocate array memory with `realloc`, \
			where `a` was %p and `el` was %s\n",
			a,
			el
		);
		return false;
	}

	a->state = next_state;
  a->state[a->size++] = cp;

	return true;
}

/**
 * @brief Deallocates memory for character array `a`
 *
 * @param a The array to deallocate
 */
void ch_array_free(ch_array_t *a) {
  free(a->state);
	// Mitigate potential memory corruption if dangling pointer is accessed
  a->state = NULL;
}

/**
 * @brief Allocates memory for a new void pointer array and its `state` member.
 *
 * @return array_t* Initialized void pointer array
 */
array_t *array_init() {
	array_t *a = malloc(sizeof(array_t));
	if (a == NULL) {
		return NULL;
	}

  a->state = malloc(sizeof(void*));
	if (a->state == NULL) {
		return NULL;
	}

  a->size = 0;

	return a;
}

/**
 * @brief Inserts a void pointer element into the given void pointer array `a`
 *
 * @param a The void pointer array into which `el` will be inserted
 * @param el The void pointer element to insert
 *
 * @return A boolean indicating whether the insertion succeeded
 */
bool array_insert(array_t *a, void *el) {
	void **next_state = realloc(a->state, (a->size + 1) * sizeof(void*));
	if (next_state == NULL) {
		LOG(
			"[array::array_insert] failed to reallocate array memory with `realloc`, \
			where `a` was %p and `el` was %s\n",
			a,
			el
		);
		return false;
	}

	a->state = next_state;
  a->state[a->size++] = el;

	return true;
}

/**
 * @brief Deallocates memory for array `a`
 *
 * @param a The array to deallocate
 */
void array_free(array_t *a) {
  free(a->state);
	// Mitigate potential memory corruption if dangling pointer is accessed
  a->state = NULL;
}
