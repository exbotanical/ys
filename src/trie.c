#include "trie.h"

#include <pcre.h> /* This library relies on the ability to utilize PCRE regex. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "libutil/libutil.h"
#include "logger.h"
#include "path.h"

/**
 * @brief Allocates memory for a new node, its children and action members.
 *
 * @return node_t*
 */
static node_t *node_init() {
  node_t *node = malloc(sizeof(node_t));
  if (!node) {
    free(node);
    DIE(EXIT_FAILURE, "[trie::node_init] %s\n", "failed to allocate node");
  }

  node->children = h_init_table(0);
  if (!node->children) {
    free(node->children);
    free(node);
    DIE(EXIT_FAILURE, "[trie::node_init] %s\n",
        "failed to initialize hash table for node children");
  }

  node->actions = h_init_table(0);
  if (!node->actions) {
    free(node->actions);
    free(node);
    DIE(EXIT_FAILURE, "[trie::node_init] %s\n",
        "failed to initialize hash table for node actions");
  }

  return node;
}

trie_t *trie_init() {
  trie_t *trie = malloc(sizeof(trie_t));
  if (trie == NULL) {
    free(trie);
    DIE(EXIT_FAILURE, "[trie::trie_init] %s\n", "failed to allocate trie_t");
  }

  trie->root = node_init();
  trie->regex_cache = h_init_table(0);
  if (!trie->regex_cache) {
    free(trie->regex_cache);
    free(trie);
    DIE(EXIT_FAILURE, "[trie::trie_init] %s\n",
        "failed to initialize hash table for trie regex cache");
  }

  return trie;
}

void trie_insert(trie_t *trie, ch_array_t *methods, const char *path,
                 void *(*handler)(void *)) {
  node_t *curr = trie->root;

  // Handle root path
  if (strcmp(path, PATH_ROOT) == 0) {
    curr->label = strdup(path);

    for (int i = 0; i < (int)methods->size; i++) {
      action_t *action = malloc(sizeof(action_t));
      if (!action) {
        free(action);
        DIE(EXIT_FAILURE, "[trie::trie_insert] %s\n",
            "failed to allocate action");
      }

      action->handler = handler;

      h_insert(curr->actions, methods->state[i], action);
    }

    return;
  }

  ch_array_t *ca = expand_path(path);
  for (int i = 0; i < (int)ca->size; i++) {
    char *split_path = ca->state[i];

    h_record *next = h_search(curr->children, split_path);
    if (next) {
      curr = next->value;
    } else {
      node_t *node = node_init();

      node->label = split_path;
      h_insert(curr->children, split_path, node);
      curr = node;
    }

    // Overwrite existing data on last path
    if (i == (int)ca->size - 1) {
      curr->label = split_path;

      for (int k = 0; k < (int)(methods->size); k++) {
        char *method = methods->state[k];

        action_t *action = malloc(sizeof(action_t));
        if (!action) {
          free(action);
          DIE(EXIT_FAILURE, "[trie::trie_insert] %s\n",
              "failed to allocate action");
        }

        action->handler = handler;
        h_insert(curr->actions, method, action);
      }

      break;
    }
  }
}

result_t *trie_search(trie_t *trie, char *method, const char *search_path) {
  result_t *result = malloc(sizeof(result_t));
  if (!result) {
    free(result);
    DIE(EXIT_FAILURE, "[trie::trie_search] %s\n", "failed to allocate result");
  }

  result->parameters = array_init();
  if (!result->parameters) {
    free(result->parameters);
    DIE(EXIT_FAILURE, "[trie::trie_search] %s\n",
        "failed to allocate result->parameters via array_init");
  }
  result->flags = INITIAL_FLAG_STATE;

  node_t *curr = trie->root;

  ch_array_t *ca = expand_path(search_path);
  for (int i = 0; i < (int)ca->size; i++) {
    char *path = ca->state[i];

    h_record *next = h_search(curr->children, path);
    if (next) {
      curr = next->value;
      continue;
    }

    if (curr->children->count == 0) {
      if (strcmp(curr->label, path) != 0) {
        // No matching route result found
        result->flags |= NOT_FOUND_MASK;
        return result;
      }
      break;
    }

    bool is_param_match = false;

    for (int k = 0; k < curr->children->capacity; k++) {
      h_record *child_record = curr->children->records[k];
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

        int ovecsize = 30;
        int ovector[ovecsize];

        if (pcre_exec(re, NULL, path, strlen(path), 0, 0, ovector, ovecsize) <
            0) {
          // No parameter match
          result->flags |= NOT_FOUND_MASK;
          return result;
        }

        char *param_key = derive_parameter_key(child->label);

        parameter_t *param = malloc(sizeof(parameter_t));
        if (!param) {
          free(param);
          DIE(EXIT_FAILURE, "[trie::trie_search] %s\n",
              "failed to allocate param");
        }

        param->key = param_key;
        param->value = path;

        if (!array_push(result->parameters, param)) {
          char *message =
              "failed to insert parameter record in result->parameters";
          LOG("[trie::trie_search] %s\n", message);
          STDERR("%s\n", message);
        }

        h_record *next = h_search(curr->children, child->label);
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

  if (strcmp(search_path, PATH_ROOT) == 0) {
    // No matching handler
    if (curr->actions->count == 0) {
      result->flags |= NOT_FOUND_MASK;
      return result;
    }
  }

  h_record *action_record = h_search(curr->actions, method);
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
