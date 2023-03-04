#ifndef ROUTER_H
#define ROUTER_H

#include "libhttp.h"
#include "libutil/libutil.h"

/**
 * @brief A route record.
 */
typedef struct route {
  Array *methods;
  char *path;
  void *(*handler)(void *);
} route_t;

/**
 * @brief Matches an inbound HTTP request against a route, passing a context
 * object to the matched handler.
 *
 * @param router The router instance in which to search for routes
 * @param method The method to search for
 * @param parameters Route match parameters
 */
void router_run(router_t *router, int client_socket, request_t *r,
                Array *parameters);

/**
 * @brief Collects n methods into a character array. The variadic arguments here
 * use the sentinel variant; the list must be punctuated with NULL.
 *
 * @param method The first of n methods
 * @param ... n methods, ending with NULL
 * @return Array* List of stringified methods
 */
Array *collect_methods(http_method_t method, ...);

#endif /* ROUTER_H */
