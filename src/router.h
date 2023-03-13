#ifndef ROUTER_H
#define ROUTER_H

#include "libhttp.h"
#include "libutil/libutil.h"

/**
 * @brief A route record.
 */
typedef struct route {
  array_t *methods;
  char *path;
  void *(*handler)(void *);
} route_t;

/**
 * @brief Matches an inbound HTTP request against a route, passing a context
 * object to the matched handler.
 *
 * @param router The router instance in which to search for routes
 * @param method The method to search for
 * TODO:
 */
void router_run(__router_t *router, int client_socket, req_t *r);

/**
 * @brief Collects n methods into a character array. The variadic arguments here
 * use the sentinel variant; the list must be punctuated with NULL.
 *
 * @param method The first of n methods
 * @param ... n methods, ending with NULL
 * @return array_t* List of stringified methods
 */
array_t *collect_methods(http_method_t method, ...);

#endif /* ROUTER_H */
