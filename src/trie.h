#ifndef TRIE_H
#define TRIE_H

#include "array.h"

#include "lib.hash/hash.h"

#include <stdbool.h>

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
	array_t *parameters;
} result_t;

/**
 * @brief Allocates memory for a trie and its root node.
 *
 * @return trie_t*
 */
trie_t *trie_init();

/**
 * @brief Inserts a node into the trie at `path` and each method of `methods`.
 *
 * @param trie The trie instance in which to insert
 * @param methods The methods on which to create a node
 * @param path The path on which to create a node
 * @param handler The handler to be associated with the inserted node
 * @return bool A boolean indicating whether the insertion succeeded
 */
bool trie_insert(trie_t *trie, ch_array_t *methods, const char *path, void*(*handler)(void*));

/**
 * @brief Searches a trie for a node matching the given method and path
 *
 * @param trie The trie in which to perform the search
 * @param method A method to search against
 * @param search_path A path to search against
 * @return result_t* A result object, or NULL if not found
 */
result_t *trie_search(trie_t *trie, char *method, const char *search_path);

/**
 * @brief Allocates memory for a new node, its children and action members.
 *
 * @return node_t*
 */
node_t *node_init();

#endif /* TRIE_H */
