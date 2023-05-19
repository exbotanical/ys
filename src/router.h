#ifndef ROUTER_H
#define ROUTER_H

#include <stdbool.h>

#include "client.h"
#include "libutil/libutil.h"
#include "libys.h"
#include "request.h"
#include "trie.h"

/**
 * A route record
 */
typedef struct {
  array_t *methods;
  char *path;
  void *(*handler)(void *, void *);
} route_record;

typedef struct {
  bool use_cors;
  ys_route_handler *not_found_handler;
  ys_route_handler *internal_error_handler;
  ys_route_handler *method_not_allowed_handler;
  array_t *middlewares;
} router_attr_internal;

/**
 * A router / HTTP multiplexer
 */
typedef struct {
  bool use_cors;
  route_trie *trie;
  ys_route_handler *not_found_handler;
  ys_route_handler *method_not_allowed_handler;
  ys_route_handler *internal_error_handler;

  // middleware_handler*[]
  array_t *middlewares;
  // <char*, router_internal*>
  hash_table *sub_routers;
} router_internal;

/**
 * router_run matches an inbound HTTP request against a route and executes the
 * appropriate handler
 */
void router_run(router_internal *router, client_context *ctx,
                request_internal *req);

#endif /* ROUTER_H */
