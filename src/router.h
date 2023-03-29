#ifndef ROUTER_H
#define ROUTER_H

#include <stdbool.h>

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
 * A router / HTTP multiplexer
 */
typedef struct {
  bool use_cors;
  route_trie *trie;
  route_handler *not_found_handler;
  route_handler *method_not_allowed_handler;
  route_handler *internal_error_handler;
  array_t *middlewares;
} router_internal;

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
