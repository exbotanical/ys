#ifndef ROUTER_H
#define ROUTER_H

#include "libhttp.h"
#include "libutil/libutil.h"

// A route record
typedef struct route {
  array_t *methods;
  char *path;
  void *(*handler)(void *, void *);
} route_t;

/**
 * router_run matches an inbound HTTP request against a route and executes the
 * appropriate handler
 *
 * @param router
 * @param client_socket
 * @param r
 */
void router_run(__router_t *router, int client_socket, req_t *r);

#endif /* ROUTER_H */
