#include "trie.h"

#include "path.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

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

			action_t *aa = malloc(sizeof(action_t));
			aa->handler = handler;

			h_insert(curr->actions, methods->state[i], aa);
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

// search searches a given path and method in the trie's routing results.
result_t *trie_search(trie_t *trie, char *method, const char* search_path) {
	result_t *result = malloc(sizeof(result_t)); // TODO: validate

	node_t *curr = trie->root;

	ch_array_t *ca = expand_path(search_path);
	for (int i = 0; i < ca->size; i++) {
		char *path = ca->state[i];

		h_record* next = h_search(curr->children, path);
		if (next) {
			curr = next->value;
			continue;
		}

		if (curr->children->count == 0) {
			if (strcmp(curr->label, path) != 0) {
				// No matching route result found.
				return NULL; // TODO: ErrNotFound
			}
			break;
		}

		bool is_param_match = false;

		for (int k = 0; k < curr->children->count; k++) {
			node_t *child = curr->children->records[i];
			char *child_label = child->label[0];

			if ((strcmp(child_label, PARAMETER_DELIMITER) == 0)) {
				char *pattern = derive_label_pattern(child_label);
				if (!pattern) {
					return NULL; // TODO: ErrNotFound
				}

				regex_t regex;
				int ret = regcomp(&regex, pattern, 0);
				if (ret) {
					return NULL; // TODO: ErrNotFound
				}

				bool matches = regexec(&regex, path, 0, NULL, 0);
				if (matches != REG_NOMATCH) {
					char *param_key = derive_parameter_key(child_label);

					parameter_t *param = malloc(sizeof(parameter_t)); // TODO: validate
					param->key = param;
					param->value = path;

					ch_array_insert(result->parameters, param);

					h_record *next = h_search(curr->children, child_label);
					if (!next) {
						printf("\n!!! no next\n"); // TODO: log and validate
					}

					curr = next;
					is_param_match = true;

					break;
				}

				// No parameter match.
				return NULL; // TODO: ErrNotFound
			}
		}

		// No parameter match.
		if (!is_param_match) {
			return NULL; // TODO: ErrNotFound
		}
	}

	if (strcmp(search_path, PATH_ROOT) == 0) {
		// No matching handler.
		if (curr->actions->count == 0) {
			return NULL; // TODO: ErrNotFound
		}
	}

	h_record *action_record = h_search(curr->actions, method); // TODO: validate
	action_t *next_action = action_record->value;

	// No matching handler.
	if (next_action == NULL) {
		return NULL; // TODO: ErrMethodNotAllowed
	}

	result->action = next_action;

	return result;
}
