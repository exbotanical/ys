#include "trie.h"

#include <pcre.h>    // This library relies on the ability to utilize PCRE regex
#include <string.h>  // for strdup, strlen

#include "cache.h"  // for regex_cache_get
#include "logger.h"
#include "path.h"
#include "util.h"
#include "xmalloc.h"

/**
 * node_init allocates memory for a new node, its children and action members
 *
 * @return node_t*
 */
static node_t *node_init() {
  node_t *node = xmalloc(sizeof(node_t));

  node->children = ht_init(0);
  if (!node->children) {
    free(node->children);
    free(node);
    DIE(EXIT_FAILURE, "[trie::node_init] %s\n",
        "failed to initialize hash table for node children");
  }

  node->actions = ht_init(0);
  if (!node->actions) {
    free(node->actions);
    free(node);
    DIE(EXIT_FAILURE, "[trie::node_init] %s\n",
        "failed to initialize hash table for node actions");
  }

  return node;
}

trie_t *trie_init() {
  trie_t *trie = xmalloc(sizeof(trie_t));

  trie->root = node_init();
  trie->regex_cache = ht_init(0);
  if (!trie->regex_cache) {
    free(trie->regex_cache);
    free(trie);
    DIE(EXIT_FAILURE, "[trie::trie_init] %s\n",
        "failed to initialize hash table for trie regex cache");
  }

  return trie;
}

void trie_insert(trie_t *trie, array_t *methods, const char *path,
                 void *(*handler)(void *, void *), array_t *middlewares) {
  node_t *curr = trie->root;

  // Handle root path
  if (str_equals(path, PATH_ROOT)) {
    curr->label = strdup(path);

    for (unsigned int i = 0; i < array_size(methods); i++) {
      action_t *action = xmalloc(sizeof(action_t));

      action->handler = handler;
      action->middlewares = middlewares;

      ht_insert(curr->actions, array_get(methods, i), action);
    }

    return;
  }

  array_t *paths = expand_path(path);
  for (unsigned int i = 0; i < array_size(paths); i++) {
    char *split_path = array_get(paths, i);
    ht_record *next = ht_search(curr->children, split_path);
    if (next) {
      curr = next->value;
    } else {
      node_t *node = node_init();

      node->label = split_path;
      ht_insert(curr->children, split_path, node);
      curr = node;
    }

    // Overwrite existing data on last path
    if (i == array_size(paths) - 1) {
      curr->label = split_path;

      for (unsigned int k = 0; k < array_size(methods); k++) {
        char *method = array_get(methods, k);

        action_t *action = xmalloc(sizeof(action_t));

        action->handler = handler;
        action->middlewares = middlewares;

        ht_insert(curr->actions, method, action);
      }

      break;
    }
  }
}

result_t *trie_search(trie_t *trie, const char *method,
                      const char *search_path) {
  result_t *result = xmalloc(sizeof(result_t));

  result->parameters = array_init();
  if (!result->parameters) {
    free(result->parameters);
    DIE(EXIT_FAILURE, "[trie::trie_search] %s\n",
        "failed to allocate result->parameters via array_init");
  }
  result->flags = INITIAL_FLAG_STATE;

  node_t *curr = trie->root;

  array_t *paths = expand_path(search_path);
  for (unsigned int i = 0; i < array_size(paths); i++) {
    char *path = array_get(paths, i);

    ht_record *next = ht_search(curr->children, path);
    if (next) {
      curr = next->value;
      continue;
    }

    if (curr->children->count == 0) {
      if (!str_equals(curr->label, path)) {
        // No matching route result found
        result->flags |= NOT_FOUND_MASK;
        return result;
      }
      break;
    }

    bool is_param_match = false;

    for (int k = 0; k < curr->children->capacity; k++) {
      ht_record *child_record = curr->children->records[k];
      if (!child_record) {
        continue;
      }

      node_t *child = child_record->value;
      char child_prefix = child->label[0];

      if (child_prefix == PARAMETER_DELIMITER[0]) {
        char *pattern = derive_label_pattern(child->label);
        if (!pattern) {
          result->flags |= NOT_FOUND_MASK;
          return result;
        }

        pcre *re = regex_cache_get(trie->regex_cache, pattern);
        if (!re) {
          LOG("[trie::trie_search] %s\n", "regex was NULL");
          return NULL;  // 500
        }

        int ovecsize = 30;  // TODO: size
        int ovector[ovecsize];

        if (pcre_exec(re, NULL, path, strlen(path), 0, 0, ovector, ovecsize) <
            0) {
          // No parameter match
          result->flags |= NOT_FOUND_MASK;
          return result;
        }

        char *param_key = derive_parameter_key(child->label);

        parameter_t *param = xmalloc(sizeof(parameter_t));

        param->key = param_key;
        param->value = path;

        if (!array_push(result->parameters, param)) {
          char *message =
              "failed to insert parameter record in result->parameters";
          LOG("[trie::trie_search] %s\n", message);
        }

        ht_record *next = ht_search(curr->children, child->label);
        if (!next) {
          LOG("[trie::trie_search] %s %s\n",
              "did not match a route but expected to, where label is",
              child->label);

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

  if (str_equals(search_path, PATH_ROOT)) {
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

  action_t *next_action = action_record->value;
  // No matching handler
  if (next_action == NULL) {
    result->flags |= NOT_ALLOWED_MASK;
    return result;
  }

  result->action = next_action;

  return result;
}
