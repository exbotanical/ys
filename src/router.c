#include "router.h"

#include <stdarg.h>  // for variadic args functions
#include <stdlib.h>  // for exit, malloc
#include <string.h>  // for strdup

#include "config.h"
#include "logger.h"
#include "server.h"  // for send_response

const char CONFIG_FILE_NAME[13] = "libhttp.conf";

static void setup_env() {
  parse_config(CONFIG_FILE_NAME);
  setup_logging();
}

// essentially a reduce operation
static void *invoke_chain(req_t *req, res_t *res, array_t *middlewares) {
  void *h = res;
  for (unsigned int i = array_size(middlewares); i > 0; i--) {
    h = ((handler_t *)array_get(middlewares, i - 1))(req, h);
  }

  return h;
}

/**
 * @brief Executes the internal 500 handler.
 *
 * @param arg Route context
 */
static void *internal_server_error_handler(req_t *req, res_t *res) {
  printlogf(LOG_INFO,
            "[router::internal_server_error_handler] 500 handler in effect at "
            "request path %s\n",
            req->path);

  res->status = INTERNAL_SERVER_ERROR;

  return res;
}

/**
 * @brief Executes the default 404 handler.
 *
 * @param arg Route context
 */
static void *default_not_found_handler(req_t *req, res_t *res) {
  printlogf(
      LOG_INFO,
      "[router::default_not_found_handler] default 404 handler in effect at "
      "request path %s\n",
      req->path);

  res->status = NOT_FOUND;

  return res;
}

/**
 * @brief Executes the default 405 handler.
 *
 * @param arg Route context
 */
static void *default_method_not_allowed_handler(req_t *req, res_t *res) {
  printlogf(
      LOG_INFO,
      "[router::default_method_not_allowed_handler] default 405 handler in "
      "effect at request path %s\n",
      req->path);

  res->status = METHOD_NOT_ALLOWED;

  return res;
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

void router_register(router_t *router, const char *path, handler_t *handler,
                     array_t *middlewares, http_method_t method, ...) {
  array_t *methods = array_init();
  if (!methods) {
    DIE(EXIT_FAILURE, "[router::collect_methods] %s\n",
        "failed to allocate methods array via array_init");
  }

  va_list args;
  va_start(args, method);
  // TODO: deduplicate this (and use collect_methods) or similar
  // not doing this right now due to reference edge cases when doing so
  while (method != 0) {
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

res_t *response_init() {
  res_t *res = malloc(sizeof(res_t));
  if (!res) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate res_t");
  }

  res->headers = array_init();
  if (!res->headers) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate array_t");
  }

  return res;
}

void router_run(__router_t *router, int client_socket, req_t *req) {
  __router_t *internal_router = (__router_t *)router;

  result_t *result = trie_search(internal_router->trie, req->method, req->path);

  res_t *res = response_init();
  if (!result) {
    res = internal_server_error_handler(req, res);
  } else if ((result->flags & NOT_FOUND_MASK) == NOT_FOUND_MASK) {
    res = internal_router->not_found_handler(req, res);
  } else if ((result->flags & NOT_ALLOWED_MASK) == NOT_ALLOWED_MASK) {
    res = internal_router->method_not_allowed_handler(req, res);
  } else {
    req->parameters = result->parameters;

    handler_t *h = (handler_t *)result->action->handler;

    array_t *mws = result->action->middlewares;
    if (mws && array_size(mws) > 0) {
      res = invoke_chain(req, res, mws);
    }

    res = h(req, res);
  }

  send_response(client_socket, res);
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

  while (method != 0) {
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
