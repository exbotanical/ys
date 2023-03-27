#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#include "libhash/libhash.h"  // for hash tables impl
#include "libutil/libutil.h"  // for array_t
#include "logger.h"

// Initial state for route result record
static const unsigned int INITIAL_FLAG_STATE = 0x00;

// Route not found flag
static const unsigned int NOT_FOUND_MASK = 0x01;

// Route not allowed flag
static const unsigned int NOT_ALLOWED_MASK = 0x02;

typedef struct {
  char *label;
  hash_table *children;
  hash_table *actions;
} node_t;

// A trie data structure used for routing
typedef struct {
  node_t *root;
  hash_table *regex_cache;
} trie_t;

// Stores a route's handler
typedef struct {
  void *(*handler)(void *, void *);
} action_t;

// Trie search result record
typedef struct {
  action_t *action;
  // hash_table<char*, char*>
  hash_table *parameters;
  // hash_table<char*, hash_set<char*>>
  hash_table *queries;
  unsigned int flags;
} result_t;

/**
 * trie_init allocates memory for a trie and its root node
 *
 * @return trie_t*
 */
trie_t *trie_init();

/**
 * trie_insert inserts a node into the trie at `path` and each method of
 * `methods`.
 *
 * @param trie The trie instance in which to insert
 * @param methods The methods on which to create a node
 * @param path The path on which to create a node
 * @param handler The handler to be associated with the inserted node
 */
void trie_insert(trie_t *trie, array_t *methods, const char *path,
                 void *(*handler)(void *, void *));

/**
 * trie_search searches a trie for a node matching the given method and path
 *
 * @param trie The trie in which to perform the search
 * @param method A method to search against
 * @param search_path A path to search against
 * @return result_t* A result object, or NULL if not found
 */
result_t *trie_search(trie_t *trie, const char *method,
                      const char *search_path);

#endif /* TRIE_H */
