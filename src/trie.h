#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#include "libhash/libhash.h"  // for hash tables impl
#include "libutil/libutil.h"  // for array_t
#include "logger.h"

// Initial state for route result record
extern const unsigned int INITIAL_FLAG_STATE;

// Route not found flag
extern const unsigned int NOT_FOUND_MASK;

// Route not allowed flag
extern const unsigned int NOT_ALLOWED_MASK;

typedef struct {
  char *label;
  hash_table *children;
  hash_table *actions;
} trie_node;

// A trie data structure used for routing
typedef struct {
  trie_node *root;
  hash_table *regex_cache;
} route_trie;

// Stores a route's handler
typedef struct {
  void *(*handler)(void *, void *);
} route_action;

// Trie search result record
typedef struct {
  route_action *action;
  // hash_table<char*, char*>
  hash_table *parameters;
  // hash_table<char*, hash_set<char*>>
  hash_table *queries;
  unsigned int flags;
} route_result;

/**
 * trie_init allocates memory for a trie and its root node
 *
 * @return route_trie*
 */
route_trie *trie_init();

/**
 * trie_insert inserts a node into the trie at `path` and each method of
 * `methods`.
 *
 * @param trie The trie instance in which to insert
 * @param methods The methods on which to create a node
 * @param path The path on which to create a node
 * @param handler The handler to be associated with the inserted node
 */
void trie_insert(route_trie *trie, array_t *methods, const char *path,
                 void *(*handler)(void *, void *));

/**
 * trie_search searches a trie for a node matching the given method and path
 *
 * @param trie The trie in which to perform the search
 * @param method A method to search against
 * @param search_path A path to search against
 * @return route_result* A result object, or NULL if not found
 */
route_result *trie_search(route_trie *trie, const char *method,
                          const char *search_path);

#endif /* TRIE_H */
