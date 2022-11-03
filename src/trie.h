#ifndef TRIE_H
#define TRIE_H

#include "array.h"

#include "lib.hash/hash.h" /* for TODO: */

typedef struct node {
	char *label;
	h_table *children;
	h_table *actions;
} node_t;

typedef struct trie {
	node_t *root;
} trie_t;

typedef struct action {
	void*(*handler)(void*);
} action_t;

typedef struct parameter {
	char *key;
	char *value;
} parameter_t;

typedef struct result {
	action_t *action;
	ch_array_t *parameters[];
} result_t;


trie_t *trie_init();

void trie_insert(trie_t *trie, ch_array_t *methods, const char* path, void*(*handler)(void*));

result_t *trie_search(trie_t *trie, char *method, const char* search_path);
result_t *result_init();
node_t *node_init();

#endif /* TRIE_H */
