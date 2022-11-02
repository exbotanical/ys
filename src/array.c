#include "array.h"

#include <stdlib.h>
#include <string.h>

ch_array_t *ch_array_init(size_t initial_size) {
	ch_array_t *a = malloc(sizeof(ch_array_t));
	if (a == NULL) {
		return NULL;
	}

  a->state = malloc(initial_size * sizeof(char*));
	if (a->state == NULL) {
		return NULL;
	}

  a->size = initial_size;

	return a;
}

// TODO: document, make safe w/null checks (for all functions)
void ch_array_insert(ch_array_t *a, char *el) {
	char *cp = strdup(el);
	if (cp == NULL) {
		return; // TODO: retval
	}

	char** next_state = realloc(a->state, (a->size + 1) * sizeof(char*));
	if (next_state == NULL) {
		return; // TODO: retval
	}

	a->state = next_state;
  a->state[a->size++] = cp;
}

void ch_array_free(ch_array_t *a) {
  free(a->state);
  a->state = NULL;
}
