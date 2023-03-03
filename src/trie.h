#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#include "array.h"
#include "libhash/libhash.h"
#include "libutil/libutil.h"

/**
 * @brief Initial flags state for result record.
 */
static const unsigned int INITIAL_FLAG_STATE = 0x00;
static const unsigned int NOT_FOUND_MASK = 0x01;
static const unsigned int NOT_ALLOWED_MASK = 0x02;

/**
 * @brief A trie node.
 */
typedef struct node {
  char *label;
  h_table *children;
  h_table *actions;
} node_t;

/**
 * @brief A trie data structure used for routing.
 */
typedef struct trie {
  node_t *root;
  h_table *regex_cache;
} trie_t;

/**
 * @brief A route handler.
 */
typedef struct action {
  void *(*handler)(void *);
} action_t;

/**
 * @brief Parameters collected from a route match.
 */
typedef struct parameter {
  char *key;
  char *value;
} parameter_t;

/**
 * @brief A trie search result.
 */
typedef struct result {
  action_t *action;
  Array *parameters;
  unsigned int flags;
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
 */
void trie_insert(trie_t *trie, ch_array_t *methods, const char *path,
                 void *(*handler)(void *));

/**
 * @brief Searches a trie for a node matching the given method and path
 *
 * @param trie The trie in which to perform the search
 * @param method A method to search against
 * @param search_path A path to search against
 * @return result_t* A result object, or NULL if not found
 */
result_t *trie_search(trie_t *trie, char *method, const char *search_path);

#endif /* TRIE_H */
