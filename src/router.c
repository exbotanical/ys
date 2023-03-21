#include "router.h"

#include <stdarg.h>  // for variadic args functions
#include <string.h>  // for strdup

#include "config.h"
#include "logger.h"
#include "response.h"  // for response_init, send_response
#include "server.h"    // for send_response
#include "xmalloc.h"

static const char CONFIG_FILE_NAME[13] = "libhttp.conf";

/**
 * setup_env configures the `server_config` and logger for the current runtime
 */
static void setup_env() {
  parse_config(CONFIG_FILE_NAME);
  setup_logging();
}

/**
 * invoke_chain reduces the route handler and middlewares into a final response
 * object that is then sent over the socket
 *
 * @param req
 * @param res
 * @param middlewares
 * @return void* The response pointer
 */
static void *invoke_chain(req_t *req, res_t *res, array_t *middlewares) {
  for (unsigned int i = array_size(middlewares); i > 0; i--) {
    res = ((handler_t *)array_get(middlewares, i - 1))(req, res);
    if (res->done) break;
  }

  return res;
}

/**
 * default_internal_error_handler is the internal/default 500 handler
 *
 * @param req
 * @param res
 * @return void*
 */
static res_t *default_internal_error_handler(req_t *req, res_t *res) {
  printlogf(LOG_INFO,
            "[router::default_internal_error_handler] 500 handler in effect at "
            "request path %s\n",
            req->path);

  res->status = STATUS_INTERNAL_SERVER_ERROR;

  return res;
}

/**
 * default_not_found_handler is the internal/default 404 handler
 *
 * @param req
 * @param res
 * @return void*
 */
static res_t *default_not_found_handler(req_t *req, res_t *res) {
  printlogf(
      LOG_INFO,
      "[router::default_not_found_handler] default 404 handler in effect at "
      "request path %s\n",
      req->path);

  res->status = STATUS_NOT_FOUND;

  return res;
}

/**
 * default_method_not_allowed_handler is the internal/default 405 handler
 *
 * @param req
 * @param res
 * @return void*
 */
static res_t *default_method_not_allowed_handler(req_t *req, res_t *res) {
  // TODO: fix
  // printlogf(
  //     LOG_INFO,
  //     "[router::default_method_not_allowed_handler] default 405 handler in "
  //     "effect at request path %s\n",
  //     req->path);

  res->status = STATUS_METHOD_NOT_ALLOWED;

  return res;
}

router_t *router_init(router_attr_t attr) {
  // Initialize config opts
  setup_env();

  __router_t *router = xmalloc(sizeof(__router_t));

  router->trie = trie_init();
  router->global_middlewares = array_init();

  if (!attr.not_found_handler) {
    LOG("[router::router_init] %s\n",
        "not_found_handler is NULL, registering fallback handler");
    router->not_found_handler = default_not_found_handler;
  } else {
    router->not_found_handler = attr.not_found_handler;
  }

  if (!attr.method_not_allowed_handler) {
    LOG("[router::router_init] %s\n",
        "method_not_allowed_handler is NULL, registering fallback handler");
    router->method_not_allowed_handler = default_method_not_allowed_handler;
  } else {
    router->method_not_allowed_handler = attr.method_not_allowed_handler;
  }

  if (!attr.internal_error_handler) {
    LOG("[router::router_init] %s\n",
        "internal_error_handler is NULL, registering fallback handler");
    router->internal_error_handler = default_internal_error_handler;
  } else {
    router->internal_error_handler = attr.internal_error_handler;
  }

  return (router_t *)router;
}

void router_register(router_t *router, const char *path, handler_t *handler,
                     middlewares_t *middlewares, http_method_t method, ...) {
  array_t *methods = array_init();
  if (!methods) {
    DIE(EXIT_FAILURE, "[router::router_register] %s\n",
        "failed to allocate methods array via array_init");
  }

  va_list args;
  va_start(args, method);
  // TODO: deduplicate this (and use collect) or similar
  // not doing this right now due to reference edge cases when doing so
  while (method != 0) {
    if (!array_push(methods, strdup(http_method_names[method]))) {
      free(methods);
      DIE(EXIT_FAILURE, "[router::router_register] %s\n",
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
              (array_t *)middlewares);
}

void router_run(__router_t *router, int client_socket, req_t *req) {
  __router_t *internal_router = (__router_t *)router;
  result_t *result = trie_search(internal_router->trie, req->method, req->path);

  res_t *res = response_init();

  if (!result) {
    res = internal_router->internal_error_handler(req, res);
  } else if ((result->flags & NOT_FOUND_MASK) == NOT_FOUND_MASK) {
    res = internal_router->not_found_handler(req, res);
  } else if ((result->flags & NOT_ALLOWED_MASK) == NOT_ALLOWED_MASK) {
    res = internal_router->method_not_allowed_handler(req, res);
  } else {
    req->parameters = result->parameters;

    handler_t *h = (handler_t *)result->action->handler;

    array_t *mws = router->global_middlewares;
    if (mws && array_size(mws) > 0) {
      res = invoke_chain(req, res, mws);
    }

    res = h(req, res);
  }

  send_response(client_socket, serialize_response(req, res));
}

void router_free(router_t *router) { free(router); }
