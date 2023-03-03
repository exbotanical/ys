#ifndef ROUTER_H
#define ROUTER_H

#include <stdbool.h>

#include "array.h"
#include "libhash/libhash.h"
#include "libutil/libutil.h"
#include "request.h"
#include "trie.h"
#include "util.h"

/**
 * @brief A router object.
 */
typedef struct router {
  trie_t *trie;
  void *(*not_found_handler)(void *);
  void *(*method_not_allowed_handler)(void *);
} router_t;

/**
 * @brief A route record.
 */
typedef struct route {
  ch_array_t *methods;
  char *path;
  void *(*handler)(void *);
} route_t;

/**
 * @brief A context object containing metadata to be passed
 * to matched route handlers.
 */
typedef struct route_context {
  int client_socket;
  char *path;
  char *method;
  char *protocol;
  char *host;
  char *user_agent;
  char *accept;
  char *content_len;
  char *content_type;
  char *content;
  char *raw;
  Array *parameters;
} route_context_t;

/**
 * @brief Initializes an object containing request metadata for a matched route.
 *
 * @param client_socket
 * @param request
 * @param parameters Any parameters derived from the matched route
 * @return route_context_t* Route context, or NULL if memory allocation failed
 */
route_context_t *route_context_init(int client_socket, request_t *r,
                                    Array *parameters);

/**
 * @brief Allocates memory for a new router and its `trie` member;
 * sets the handlers for 404 and 405 (if none provided, defaults will be
 * used).
 *
 * @param not_found_handler
 * @param method_not_allowed_handler
 * @return router_t*
 */
router_t *router_init(void *(*not_found_handler)(void *),
                      void *(*method_not_allowed_handler)(void *));

/**
 * @brief Registers a new route record. Registered routes will be matched
 * against when used with `router_run`.
 *
 * @param router The router instance in which to register the route
 * @param methods Methods to associate with the route
 * @param path The path to associate with the route
 * @param handler The handler to associate with the route
 *
 */
void router_register(router_t *router, ch_array_t *methods, const char *path,
                     void *(*handler)(void *));

/**
 * @brief Matches an inbound HTTP request against a route, passing route_context
 * to the matched handler.
 *
 * @param router The router instance in which to search for routes
 * @param method The method to search for
 * @param path The path to search for
 * @return bool A boolean indicating whether a route record was matched and
 * executed
 */
void router_run(router_t *router, route_context_t *context);

/**
 * @brief Deallocates memory for router_t `router`.
 *
 * @param router The router to deallocate
 */
void router_free(router_t *router);

/**
 * @brief Collects n methods into a character array. The variadic arguments here
 * use the sentinel variant; the list must be punctuated with NULL.
 *
 * @param method The first of n methods
 * @param ... n methods, ending with NULL
 * @return ch_array_t*
 */
ch_array_t *collect_methods(char *method, ...);

#endif /* ROUTER_H */
