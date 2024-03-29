#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#include "libhash/libhash.h"
#include "libutil/libutil.h"
#include "logger.h"

// Initial state for route result record
extern const unsigned int INITIAL_FLAG_STATE;

// Route not found flag
extern const unsigned int NOT_FOUND_MASK;

// Route not allowed flag
extern const unsigned int NOT_ALLOWED_MASK;

typedef void *generic_handler(void *, void *);

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
  generic_handler *handler;
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
 */
route_trie *trie_init(void);

/**
 * trie_insert inserts a node into the trie at `path` and each method of
 * `methods`
 */
void trie_insert(route_trie *trie, array_t *methods, const char *path,
                 generic_handler *handler);

/**
 * trie_search searches a trie for a node matching the given method and path and
 * returns a result object, or NULL if not found
 */
route_result *trie_search(route_trie *trie, const char *method,
                          const char *search_path);

#endif /* TRIE_H */
