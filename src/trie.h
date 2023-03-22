#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#include "libhash/libhash.h"  // for hash tables impl
#include "libutil/libutil.h"  // for array_t

// Initial state for route result record
static const unsigned int INITIAL_FLAG_STATE = 0x00;

// Route not found flag
static const unsigned int NOT_FOUND_MASK = 0x01;

// Route not allowed flag
static const unsigned int NOT_ALLOWED_MASK = 0x02;

typedef struct node {
  char *label;
  hash_table *children;
  hash_table *actions;
} node_t;

// A trie data structure used for routing
typedef struct trie {
  node_t *root;
  hash_table *regex_cache;
} trie_t;

// Stores a route's handler
typedef struct action {
  void *(*handler)(void *, void *);
} action_t;

// Parameter collected from a route match
typedef struct parameter {
  char *key;
  char *value;
} parameter_t;

// Trie search result record
typedef struct result {
  action_t *action;
  array_t *parameters;
  unsigned int flags;
} result_t;

/**
 * Allocates memory for a trie and its root node
 *
 * @return trie_t*
 */
trie_t *trie_init();

/**
 * Inserts a node into the trie at `path` and each method of `methods`.
 *
 * @param trie The trie instance in which to insert
 * @param methods The methods on which to create a node
 * @param path The path on which to create a node
 * @param handler The handler to be associated with the inserted node
 */
void trie_insert(trie_t *trie, array_t *methods, const char *path,
                 void *(*handler)(void *, void *));

/**
 * Searches a trie for a node matching the given method and path
 *
 * @param trie The trie in which to perform the search
 * @param method A method to search against
 * @param search_path A path to search against
 * @return result_t* A result object, or NULL if not found
 */
result_t *trie_search(trie_t *trie, const char *method,
                      const char *search_path);

#endif /* TRIE_H */
