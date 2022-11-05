#include "array.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

ch_array_t *ch_array_init() {
	ch_array_t *a = malloc(sizeof(ch_array_t));
	if (a == NULL) {
		free(a);

		LOG(
			"[array::ch_array_init] %s\n",
			"failed to allocate ch_array_t"
		);

		return NULL;
	}

  a->state = malloc(sizeof(char*));
	if (a->state == NULL) {
		ch_array_free(a);

		LOG(
			"[array::ch_array_init] %s\n",
			"failed to allocate ch_array_t::state"
		);

		return NULL;
	}

  a->size = 0;

	return a;
}

bool ch_array_insert(ch_array_t *a, char *el) {
	char *cp = strdup(el);
	if (cp == NULL) {
		free(cp);

    LOG(
			"[array::ch_array_insert] %s\n",
			"failed to copy provided element with `strdup`"
		);

		return false;
	}

	char **next_state = realloc(a->state, (a->size + 1) * sizeof(char*));
	if (next_state == NULL) {
		free(cp);
		free(next_state);

		LOG(
			"[array::ch_array_insert] %s\n",
			"failed to reallocate array memory with `realloc`"
		);

		return false;
	}

	a->state = next_state;
  a->state[a->size++] = cp;

	return true;
}

void ch_array_free(ch_array_t *a) {
  free(a->state);
	// Mitigate potential memory corruption if dangling pointer is accessed
  a->state = NULL;
	free(a);
}

array_t *array_init() {
	array_t *a = malloc(sizeof(array_t));
	if (a == NULL) {
		free(a);

		LOG("[array::array_init] %s\n", "failed to allocate array_t");

		return NULL;
	}

  a->state = malloc(sizeof(void*));
	if (a->state == NULL) {
		array_free(a);

		LOG("[array::array_init] %s\n", "failed to allocate array_t::state");
	}

  a->size = 0;

	return a;
}

bool array_insert(array_t *a, void *el) {
	void **next_state = realloc(a->state, (a->size + 1) * sizeof(void*));
	if (next_state == NULL) {
		free(next_state);

		LOG(
			"[array::array_insert] %s\n",
			"failed to reallocate array memory with `realloc`"
		);

		return false;
	}

	a->state = next_state;
  a->state[a->size++] = el;

	return true;
}

void array_free(array_t *a) {
  free(a->state);
	// Mitigate potential memory corruption if dangling pointer is accessed
  a->state = NULL;
	free(a);
}
