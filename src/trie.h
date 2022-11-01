#ifndef TRIE_H
#define TRIE_H

#include "http.h"

#include "lib.hash/hash.h" /* for TODO: */

typedef struct node {
	char* label;
	h_table* children;
	h_table* actions;
} node_t;

typedef struct trie {
	node_t* root;
} trie_t;

typedef struct action {
	handler_t handler;
} action_t;

typedef struct parameter {
	char* key;
	char* value;
} parameter_t;

typedef struct result {
	action_t action;
	parameter_t parameter[];
} result_t;

typedef struct handler {

} handler_t;

trie_t* trie_init();
void trie_insert(method methods[], const char* path, handler_t* handler);
result_t* trie_search(method method, const char* search_path);
result_t* result_init();

#endif /* TRIE_H */
