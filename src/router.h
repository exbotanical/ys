#ifndef ROUTER_H
#define ROUTER_H

#include "libhttp.h"
#include "libutil/libutil.h"

/**
 * A route record
 */
typedef struct {
  array_t *methods;
  char *path;
  void *(*handler)(void *, void *);
} route_record;

/**
 * router_run matches an inbound HTTP request against a route and executes the
 * appropriate handler
 *
 * @param router
 * @param client_sockfd
 * @param r
 */
void router_run(router_internal *router, int client_sockfd, request *r);

#endif /* ROUTER_H */
