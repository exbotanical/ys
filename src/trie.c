#include "trie.h"

#include "path.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

trie_t *trie_init() {
	trie_t *trie = malloc(sizeof(trie_t));
	if (trie == NULL) {
		return NULL;
	}

	trie->root = node_init();

	return trie;
}

node_t *node_init() {
	node_t *node = malloc(sizeof(node_t));
	node->children = h_init_table(0);
	node->actions = h_init_table(0);

	return node;
}

void trie_insert(trie_t *trie, ch_array_t *methods, const char *path, void*(*handler)(void*)) {
	node_t *curr = trie->root;

	// Handle root path
	if (strcmp(path, PATH_ROOT) == 0) {
		curr->label = path;

		for (int i = 0; i < (int)methods->size; i++) {
			struct action a = {
				.handler = handler,
			};
			h_insert(curr->actions, methods->state[i], &a);
		}

		return;
	}

	ch_array_t *ca = expand_path(path);
	for (int i = 0; i < (int)ca->size; i++) {
		char *split_path = ca->state[i];

		h_record* next = h_search(curr->children, split_path);
		if (next) {
			curr = next->value;
		} else {
			node_t *node = node_init();
			h_insert(curr->children, split_path, node);
			curr = node;
		}

		// Overwrite existing data on last path
		if (i == (int)ca->size - 1) {
			curr->label = split_path;
			for (int k = 0; k < (int)(methods->size); k++) {
				char *method = methods->state[k];

				action_t action = {
					.handler = handler,
				};
				h_insert(curr->actions, method, &action);
			}

			break;
		}
	}
}
