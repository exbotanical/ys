#include "router.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "config.h"
#include "libhttp.h"
#include "logger.h"
#include "server.h"

const char CONFIG_FILE_NAME[13] = "libhttp.conf";

static void setup_env() {
  parse_config(CONFIG_FILE_NAME);
  setup_logging();
}

// essentially a reduce operation
static void *invoke_chain(route_context_t *ctx, array_t *middlewares) {
  void *h = ctx;
  for (unsigned int i = array_size(middlewares); i > 0; i--) {
    h = ((void *(*)(void *))array_get(middlewares, i - 1))(h);
  }

  return h;
}

/**
 * @brief Executes the internal 500 handler.
 *
 * @param arg Route context
 * @return void* response_t
 */
static void *internal_server_error_handler(void *arg) {
  __route_context_t *context = (__route_context_t *)arg;
  printlogf(LOG_INFO,
            "[router::internal_server_error_handler] 500 handler in effect at "
            "request path %s\n",
            context->path);

  response_t *response = response_init();
  response->status = INTERNAL_SERVER_ERROR;

  return response;
}

/**
 * @brief Executes the default 404 handler.
 *
 * @param arg Route context
 * @return void* response_t
 */
static void *default_not_found_handler(void *arg) {
  __route_context_t *context = (__route_context_t *)arg;
  printlogf(
      LOG_INFO,
      "[router::default_not_found_handler] default 404 handler in effect at "
      "request path %s\n",
      context->path);

  response_t *response = response_init();
  response->status = NOT_FOUND;

  return response;
}

/**
 * @brief Executes the default 405 handler.
 *
 * @param arg Route context
 * @return void* response_t
 */
static void *default_method_not_allowed_handler(void *arg) {
  __route_context_t *context = (__route_context_t *)arg;
  printlogf(
      LOG_INFO,
      "[router::default_method_not_allowed_handler] default 405 handler in "
      "effect at request path %s\n",
      context->path);

  response_t *response = response_init();
  response->status = METHOD_NOT_ALLOWED;

  return response;
}

/**
 * @brief Initializes an object containing request metadata for a matched route.
 *
 * @param client_socket
 * @param request
 * @param parameters Any parameters derived from the matched route
 * @return route_context_t* Route context
 */
static route_context_t *route_context_init(int client_socket, request_t *r,
                                           array_t *parameters) {
  __route_context_t *context = malloc(sizeof(__route_context_t));
  if (!context) {
    free(context);
    DIE(EXIT_FAILURE, "[context::route_context_init] %s\n",
        "failed to allocate route_context_t");
  }

  context->client_socket = client_socket;
  context->path = r->path;
  context->method = r->method;
  context->protocol = r->protocol;
  context->host = r->host;
  context->user_agent = r->user_agent;
  context->accept = r->accept;
  context->content_len = r->content_len;
  context->content_type = r->content_type;
  context->content = r->content;
  context->raw = r->raw;
  context->parameters = parameters;

  return context;
}

router_t *router_init(void *(*not_found_handler)(void *),
                      void *(*method_not_allowed_handler)(void *)) {
  // initialize config opts
  setup_env();

  __router_t *router = malloc(sizeof(__router_t));
  if (!router) {
    router_free((router_t *)router);
    DIE(EXIT_FAILURE, "[router::router_init] %s\n",
        "failed to allocate router_t");
  }

  router->trie = trie_init();

  if (!not_found_handler) {
    LOG("[router::router_init] %s\n",
        "not_found_handler is NULL, registering fallback handler");
    router->not_found_handler = default_not_found_handler;
  } else {
    router->not_found_handler = not_found_handler;
  }

  if (!method_not_allowed_handler) {
    LOG("[router::router_init] %s\n",
        "method_not_allowed_handler is NULL, registering fallback handler");
    router->method_not_allowed_handler = default_method_not_allowed_handler;
  } else {
    router->method_not_allowed_handler = method_not_allowed_handler;
  }

  return (router_t *)router;
}

void router_register(router_t *router, const char *path,
                     void *(*handler)(void *), array_t *middlewares,
                     http_method_t method, ...) {
  array_t *methods = array_init();
  if (!methods) {
    DIE(EXIT_FAILURE, "[router::collect_methods] %s\n",
        "failed to allocate methods array via array_init");
  }

  va_list args;
  va_start(args, method);
  // TODO: deduplicate this (and use collect_methods) or similar
  // not doing this right now due to reference edge cases when doing so
  while (method != NULL) {
    if (!array_push(methods, strdup(http_method_names[method]))) {
      free(methods);
      DIE(EXIT_FAILURE, "[router::collect_methods] %s\n",
          "failed to insert into methods array");
    }
    method = va_arg(args, http_method_t);
  }

  va_end(args);

  if (!router || !methods || !path || !handler) {
    DIE(EXIT_FAILURE, "%s\n",
        "invariant violation - router_register arguments cannot be NULL");
  }

  trie_insert(((__router_t *)router)->trie, methods, path, handler,
              middlewares);
}

void router_run(__router_t *router, int client_socket, request_t *r,
                array_t *parameters) {
  __router_t *internal_router = (__router_t *)router;
  __route_context_t *context = route_context_init(client_socket, r, parameters);

  response_t *response;
  result_t *result =
      trie_search(internal_router->trie, context->method, context->path);

  if (!result) {
    response = internal_server_error_handler(context);
  } else if ((result->flags & NOT_FOUND_MASK) == NOT_FOUND_MASK) {
    response = internal_router->not_found_handler(context);
  } else if ((result->flags & NOT_ALLOWED_MASK) == NOT_ALLOWED_MASK) {
    response = internal_router->method_not_allowed_handler(context);
  } else {
    context->parameters = result->parameters;
    response_t *(*h)(void *) = result->action->handler;

    array_t *mws = result->action->middlewares;
    if (mws && array_size(mws) > 0) {
      context = invoke_chain(context, mws);
    }
    response = h(context);
  }

  send_response(context->client_socket, response);
}

void router_free(router_t *router) { free(router); }

array_t *collect_methods(http_method_t method, ...) {
  array_t *methods = array_init();
  if (!methods) {
    DIE(EXIT_FAILURE, "[router::collect_methods] %s\n",
        "failed to allocate methods array via array_init");
  }

  va_list args;
  va_start(args, method);

  while (method != NULL) {
    if (!array_push(methods, strdup(http_method_names[method]))) {
      free(methods);
      DIE(EXIT_FAILURE, "[router::collect_methods] %s\n",
          "failed to insert into methods array");
    }
    method = va_arg(args, http_method_t);
  }

  va_end(args);

  return methods;
}
