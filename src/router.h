#ifndef ROUTER_H
#define ROUTER_H

#include "libhttp.h"

/**
 * @brief A route record.
 */
typedef struct route {
  ch_array_t *methods;
  char *path;
  void *(*handler)(void *);
} route_t;

/**
 * @brief Matches an inbound HTTP request against a route, passing route_context
 * to the matched handler.
 *
 * @param router The router instance in which to search for routes
 * @param method The method to search for
 * @param path The path to search for
 *
 * TODO: docs
 * @return bool A boolean indicating whether a route record was matched and
 * executed
 */
void router_run(router_t *router, int client_socket, request_t *r,
                Array *parameters);

/**
 * @brief Collects n methods into a character array. The variadic arguments here
 * use the sentinel variant; the list must be punctuated with NULL.
 *
 * @param method The first of n methods
 * @param ... n methods, ending with NULL
 * @return ch_array_t*
 * TODO: docs
 */
ch_array_t *collect_methods(http_method_t method, ...);

#endif /* ROUTER_H */
