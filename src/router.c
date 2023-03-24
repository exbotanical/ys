#include "router.h"

#include <stdarg.h>  // for variadic args functions

#include "config.h"
#include "libutil/libutil.h"  // for s_copy, s_equals
#include "logger.h"
#include "response.h"  // for res_init, res_send
#include "xmalloc.h"

static const char CONFIG_FILE_NAME[13] = "libhttp.conf";

/**
 * setup_env configures the `server_config` and logger for the current runtime
 */
static void setup_env() {
  if (!parse_config(CONFIG_FILE_NAME)) {
    DIE(EXIT_FAILURE, "%s\n", "Invalid config file");
  }

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
static bool invoke_chain(req_t *req, res_t *res, array_t *middlewares) {
  for (unsigned int i = array_size(middlewares); i > 0; i--) {
    ((handler_t *)array_get(middlewares, i - 1))(req, res);

    if (res->done) {
      return true;
    }
  }

  return false;
}

/**
 * default_internal_error_handler is the internal/default 500 handler
 *
 * @param req
 * @param res
 * @return void*
 */
static res_t *default_internal_error_handler(req_t *req, res_t *res) {
  printlogf(LOG_INFO, "[router::%s] 500 handler in effect at request path %s\n",
            __func__, req->path);

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
  printlogf(LOG_INFO,
            "[router::%s] default 404 handler in effect "
            "at request path %s\n",
            __func__, req->path);

  res->status = STATUS_NOT_FOUND;
  res->body = NULL;
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
  printlogf(LOG_INFO,
            "[router::%s] default 405 handler in "
            "effect at request path %s\n",
            __func__, req->path);

  res->status = STATUS_METHOD_NOT_ALLOWED;

  return res;
}

router_attr_t *router_attr_init() {
  router_attr_t *attr = malloc(sizeof(router_attr_t));
  attr->use_cors = false;
  attr->middlewares = NULL;
  return attr;
}

router_t *router_init(router_attr_t *attr) {
  // Initialize config opts
  setup_env();

  __router_t *router = xmalloc(sizeof(__router_t));

  router->trie = trie_init();
  router->middlewares = attr->middlewares;
  router->use_cors = attr->use_cors;

  if (!attr->not_found_handler) {
    printlogf(LOG_DEBUG,
              "[router::%s] not_found_handler is NULL, registering fallback "
              "handler\n",
              __func__);
    router->not_found_handler = default_not_found_handler;
  } else {
    router->not_found_handler = attr->not_found_handler;
  }

  if (!attr->method_not_allowed_handler) {
    printlogf(LOG_DEBUG,
              "[router::%s] method_not_allowed_handler is NULL, registering "
              "fallback handler\n",
              __func__);
    router->method_not_allowed_handler = default_method_not_allowed_handler;
  } else {
    router->method_not_allowed_handler = attr->method_not_allowed_handler;
  }

  if (!attr->internal_error_handler) {
    printlogf(LOG_DEBUG,
              "[router::%s] internal_error_handler is NULL, registering "
              "fallback handler\n",
              __func__);
    router->internal_error_handler = default_internal_error_handler;
  } else {
    router->internal_error_handler = attr->internal_error_handler;
  }

  return (router_t *)router;
}

bool has(void *s, void *cmp) { return s_equals((char *)s, (char *)cmp); }
void router_register(router_t *router, const char *path, handler_t *handler,
                     http_method_t method, ...) {
  array_t *methods = array_init();
  if (!methods) {
    DIE(EXIT_FAILURE,
        "[router::%s] failed to allocate methods array via array_init\n",
        __func__);
  }

  va_list args;
  va_start(args, method);
  while (method != 0) {
    if (!array_push(methods, s_copy(http_method_names[method]))) {
      free(methods);
      DIE(EXIT_FAILURE, "[router::%s] failed to insert into methods array\n",
          __func__);
    }
    method = va_arg(args, http_method_t);
  }

  va_end(args);

  if (!router || !methods || !path || !handler) {
    DIE(EXIT_FAILURE,
        "[router::%s] invariant violation - router_register arguments cannot "
        "be NULL\n",
        __func__);
  }

  // OPTIONS should always be allowed if we're using CORS
  char *options = s_copy(http_method_names[METHOD_OPTIONS]);
  if (((__router_t *)router)->use_cors &&
      !array_includes(methods, has, options)) {
    array_push(methods, options);
  }

  trie_insert(((__router_t *)router)->trie, methods, path, handler);
}

void router_run(__router_t *router, int client_socket, req_t *req) {
  __router_t *internal_router = (__router_t *)router;
  result_t *result = trie_search(internal_router->trie, req->method, req->path);

  res_t *res = res_init();

  if (!result) {
    res = internal_router->internal_error_handler(req, res);
  } else if ((result->flags & NOT_FOUND_MASK) == NOT_FOUND_MASK) {
    res = internal_router->not_found_handler(req, res);
  } else if ((result->flags & NOT_ALLOWED_MASK) == NOT_ALLOWED_MASK) {
    res = internal_router->method_not_allowed_handler(req, res);
  } else {
    req->parameters = result->parameters;

    handler_t *h = (handler_t *)result->action->handler;

    // TODO: wrap other handlers - for example, if auth or CORS err, caller
    // should not see 404 and 405
    bool done = false;
    array_t *mws = router->middlewares;
    if (mws && array_size(mws) > 0) {
      done = invoke_chain(req, res, mws);
    }

    if (!done) {
      res = h(req, res);
    }
  }

  res_send(client_socket, res_serialize(req, res));
}

void router_free(router_t *router) { free(router); }
