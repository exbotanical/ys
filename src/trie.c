#include "trie.h"

#include <pcre.h>

#include "cache.h"
#include "libutil/libutil.h"
#include "logger.h"
#include "path.h"
#include "regexpr.h"
#include "url.h"
#include "util.h"
#include "xmalloc.h"

// Initial state for route result record
const unsigned int INITIAL_FLAG_STATE = 0x00;

// Route not found flag
const unsigned int NOT_FOUND_MASK = 0x01;

// Route not allowed flag
const unsigned int NOT_ALLOWED_MASK = 0x02;

static route_action *action_init(generic_handler *handler) {
  route_action *action = xmalloc(sizeof(route_action));
  action->handler = handler;

  return action;
}

static route_result *result_init(void) {
  route_result *result = xmalloc(sizeof(route_result));
  result->parameters = ht_init(0);
  result->flags = INITIAL_FLAG_STATE;

  return result;
}

/**
 * node_init allocates memory for a new node, its children and action members
 *
 * @return trie_node*
 */
static trie_node *node_init(void) {
  trie_node *node = xmalloc(sizeof(trie_node));

  node->children = ht_init(0);
  if (!node->children) {
    DIE("[trie::%s] failed to initialize hash table for node children\n",
        __func__);
  }

  node->actions = ht_init(0);
  if (!node->actions) {
    DIE("[trie::%s] failed to initialize hash table for node actions\n",
        __func__);
  }

  return node;
}

route_trie *trie_init(void) {
  route_trie *trie = xmalloc(sizeof(route_trie));

  trie->root = node_init();
  trie->regex_cache = ht_init(0);
  if (!trie->regex_cache) {
    DIE("[trie::%s] failed to initialize hash table for trie regex cache\n",
        __func__);
  }

  return trie;
}

void trie_insert(route_trie *trie, array_t *methods, const char *path,
                 generic_handler *handler) {
  char *realpath = s_copy(path);
  trie_node *curr = trie->root;

  // Handle root path
  if (s_equals(realpath, PATH_ROOT)) {
    curr->label = realpath;

    foreach (methods, i) {
      route_action *action = action_init(handler);
      ht_insert(curr->actions, array_get(methods, i), action);
    }

    return;
  }

  array_t *paths = expand_path(realpath);
  foreach (paths, i) {
    char *split_path = array_get(paths, i);

    ht_record *next = ht_search(curr->children, split_path);
    if (next) {
      curr = next->value;
    } else {
      trie_node *node = node_init();

      node->label = split_path;
      ht_insert(curr->children, split_path, node);
      curr = node;
    }

    // Overwrite existing data on last path
    if (i == array_size(paths) - 1) {
      curr->label = split_path;

      foreach (methods, j) {
        char *method = array_get(methods, j);

        route_action *action = action_init(handler);
        ht_insert(curr->actions, method, action);
      }

      break;
    }
  }

  array_free(paths);
}

route_result *trie_search(route_trie *trie, const char *method,
                          const char *search_path) {
  // Extract query string, if present
  char *realpath = s_copy(search_path);
  route_result *result = result_init();
  trie_node *curr = trie->root;

  if (has_query_string(search_path)) {
    array_t *arr = str_cut(realpath, "?");
    // Remove query from path
    realpath = array_get(arr, 0);
    // Grab query and parse it into key/value[] pairs
    char *query = array_get(arr, 1);

    if (query) {
      result->queries = parse_query(query);
    }

    array_free(arr);
  }

  array_t *paths = expand_path(realpath);
  foreach (paths, i) {
    char *path = array_get(paths, i);

    ht_record *next = ht_search(curr->children, path);
    if (next) {
      curr = next->value;
      continue;
    }

    if (curr->children->count == 0) {
      if (!s_equals(curr->label, path)) {
        // No matching route result found
        result->flags |= NOT_FOUND_MASK;
        return result;
      }
      break;
    }

    bool is_param_match = false;

    // We must iterate the capacity here because hash table records are not
    // stored contiguously
    for (int j = 0; j < curr->children->capacity; j++) {
      ht_record *child_record = curr->children->records[j];
      // If there's no record in this slot, continue
      if (!child_record) {
        continue;
      }

      trie_node *child = child_record->value;
      char child_prefix = child->label[0];

      if (child_prefix == PARAMETER_DELIMITER[0]) {
        char *pattern = derive_label_pattern(child->label);
        if (!pattern) {
          result->flags |= NOT_FOUND_MASK;
          return result;
        }

        pcre *re = regex_cache_get(trie->regex_cache, pattern);
        if (!re) {
          printlogf(LOG_INFO, "[trie::%s] regex was NULL\n", __func__);

          return NULL;  // 500
        }

        if (!regex_match(re, path)) {
          // No parameter match
          result->flags |= NOT_FOUND_MASK;
          return result;
        }

        char *param_key = derive_parameter_key(child->label);
        ht_insert(result->parameters, param_key, path);

        ht_record *next = ht_search(curr->children, child->label);
        if (!next) {
          printlogf(LOG_INFO,
                    "[trie::%s] did not match a route but expected "
                    "to, where label is %s\n",
                    __func__, child->label);

          return NULL;  // 500
        }

        curr = next->value;
        is_param_match = true;

        break;
      }
    }

    // No parameter match
    if (!is_param_match) {
      result->flags |= NOT_FOUND_MASK;
      return result;
    }
  }
  array_free(paths);

  if (s_equals(realpath, PATH_ROOT)) {
    // No matching handler
    if (curr->actions->count == 0) {
      result->flags |= NOT_FOUND_MASK;
      return result;
    }
  }

  ht_record *action_record = ht_search(curr->actions, method);
  // No matching handler
  if (action_record == NULL) {
    result->flags |= NOT_ALLOWED_MASK;
    return result;
  }

  route_action *next_action = action_record->value;
  // No matching handler
  if (next_action == NULL) {
    result->flags |= NOT_ALLOWED_MASK;
    return result;
  }

  result->action = next_action;

  return result;
}
