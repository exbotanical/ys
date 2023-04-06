#include "router.h"

#include <pcre.h>
#include <stdarg.h>
#include <string.h>

#include "config.h"
#include "libutil/libutil.h"
#include "logger.h"
#include "middleware.h"
#include "path.h"
#include "regexpr.h"
#include "response.h"
#include "xmalloc.h"

#define CR(op) (response_internal *)op
#define CRR(req, res) (request *)req, (response *)res

static const char CONFIG_FILE_NAME[] = "libhttp.conf";

/**
 * setup_env configures the `server_conf` and logger for the current runtime
 *
 * TODO: relocate
 */
static void setup_env() {
  if (!parse_config(CONFIG_FILE_NAME)) {
    DIE("Invalid config file\n");
  }

  setup_logging();
}

/**
 * invoke_chain reduces the route handler and middlewares into a final response
 * object that is then sent over the socket
 *
 * @param req
 * @param res
 * @param mws
 * @return bool indicating whether the `done` flag should be set
 */
static bool invoke_chain(request_internal *req, response_internal *res,
                         array_t *mws) {
  for (unsigned int i = array_size(mws); i > 0; i--) {
    middleware_handler *mh = (middleware_handler *)array_get(mws, i - 1);

    if (has_elements(mh->ignore_paths)) {
      foreach (mh->ignore_paths, j) {
        pcre *re = array_get(mh->ignore_paths, j);

        if (regex_match(re, req->pure_path)) {
          goto continue_outer;
        }
      }
    }

    mh->handler(CRR(req, res));

    if (res->done) {
      return true;
    }

  continue_outer:;
  }

  return false;
}

/**
 * default_internal_error_handler is the internal/default 500 handler
 *
 * @param req
 * @param res
 * @return response*
 */
static response *default_internal_error_handler(request *req, response *res) {
  printlogf(LOG_INFO, "[router::%s] 500 handler in effect at request path %s\n",
            __func__, request_get_path(req));

  set_status(res, STATUS_INTERNAL_SERVER_ERROR);

  return res;
}

/**
 * default_not_found_handler is the internal/default 404 handler
 *
 * @param req
 * @param res
 * @return response*
 */
static response *default_not_found_handler(request *req, response *res) {
  printlogf(LOG_INFO,
            "[router::%s] default 404 handler in effect "
            "at request path %s\n",
            __func__, request_get_path(req));

  set_status(res, STATUS_NOT_FOUND);

  return res;
}

/**
 * default_method_not_allowed_handler is the internal/default 405 handler
 *
 * @param req
 * @param res
 * @return response*
 */
static response *default_method_not_allowed_handler(request *req,
                                                    response *res) {
  printlogf(LOG_INFO,
            "[router::%s] default 405 handler in "
            "effect at request path %s\n",
            __func__, request_get_path(req));

  set_status(res, STATUS_METHOD_NOT_ALLOWED);

  return res;
}

/**
 * router_run_sub runs a sub-router against nested paths
 *
 * @param router
 * @param ctx
 * @param req
 * @return true A sub-route was matched and the router was run
 * @return false A sub-route was not matched and we should continue to running
 * the router on the full path
 */
static bool router_run_sub(router_internal *router, client_context *ctx,
                           request_internal *req) {
  if (router->sub_routers && router->sub_routers->count) {
    array_t *paths = path_split_first_delim(req->route_path);
    char *prefix = array_get(paths, 0);
    char *suffix = array_get(paths, 1);

    if (prefix) {
      router_internal *sub_router =
          (router_internal *)ht_get(router->sub_routers, prefix);
      if (sub_router) {
        char *sub_path = suffix ? suffix : "/";
        strcpy(req->route_path, sub_path);
        printlogf(LOG_DEBUG, "matched sub-router at path %s and sub-path %s\n",
                  prefix, req->route_path);

        router_run(sub_router, ctx, req);

        array_free(paths);
        return true;
      }
    }

    array_free(paths);
  }

  return false;
}

router_attr *router_attr_init() {
  router_attr_internal *attr = xmalloc(sizeof(router_attr_internal));
  attr->use_cors = false;
  attr->middlewares = NULL;

  return (router_attr *)attr;
}

http_router *router_init(router_attr *attr) {
  router_attr_internal *attr_internal = (router_attr_internal *)attr;

  // Initialize config opts
  setup_env();

  router_internal *router = xmalloc(sizeof(router_internal));

  router->trie = trie_init();
  router->middlewares = attr_internal->middlewares;
  router->use_cors = attr_internal->use_cors;
  router->sub_routers = NULL;

  if (!attr_internal->not_found_handler) {
    printlogf(LOG_DEBUG,
              "[router::%s] not_found_handler is NULL, registering fallback "
              "handler\n",
              __func__);

    router->not_found_handler = default_not_found_handler;
  } else {
    printlogf(LOG_DEBUG,
              "[router::%s] registering user-defined not_found_handler\n",
              __func__);

    router->not_found_handler = attr_internal->not_found_handler;
  }

  if (!attr_internal->method_not_allowed_handler) {
    printlogf(LOG_DEBUG,
              "[router::%s] method_not_allowed_handler is NULL, registering "
              "fallback handler\n",
              __func__);

    router->method_not_allowed_handler = default_method_not_allowed_handler;
  } else {
    printlogf(
        LOG_DEBUG,
        "[router::%s] registering user-defined method_not_allowed_handler\n",
        __func__);

    router->method_not_allowed_handler =
        attr_internal->method_not_allowed_handler;
  }

  if (!attr_internal->internal_error_handler) {
    printlogf(LOG_DEBUG,
              "[router::%s] internal_error_handler is NULL, registering "
              "fallback handler\n",
              __func__);

    router->internal_error_handler = default_internal_error_handler;
  } else {
    printlogf(LOG_DEBUG,
              "[router::%s] registering user-defined internal_error_handler\n",
              __func__);

    router->internal_error_handler = attr_internal->internal_error_handler;
  }

  return (http_router *)router;
}

bool has(void *s, void *cmp) { return s_equals((char *)s, (char *)cmp); }

void router_register(http_router *router, const char *path,
                     route_handler *handler, http_method method, ...) {
  array_t *methods = array_init();
  if (!methods) {
    DIE("[router::%s] failed to allocate methods array via array_init\n",
        __func__);
  }

  va_list args;
  va_start(args, method);

  while (method != 0) {
    if (!array_push(methods, s_copy(http_method_names[method]))) {
      free(methods);
      DIE("[router::%s] failed to insert into methods array\n", __func__);
    }

    method = va_arg(args, http_method);
  }

  va_end(args);

  if (!router || !methods || !path || !handler) {
    DIE("[router::%s] invariant violation - router_register arguments cannot "
        "be NULL\n",
        __func__);
  }

  // OPTIONS should always be allowed if we're using CORS
  char *options = s_copy(http_method_names[METHOD_OPTIONS]);
  if (((router_internal *)router)->use_cors &&
      !array_includes(methods, has, options)) {
    array_push(methods, options);
  }

  trie_insert(((router_internal *)router)->trie, methods, path,
              (generic_handler *)handler);
}

void router_run(router_internal *router, client_context *ctx,
                request_internal *req) {
  if (router_run_sub(router, ctx, req)) {
    return;
  }

  route_result *result =
      trie_search(router->trie, req->method, req->route_path);

  response_internal *res = response_init();

  if (!result) {
    res = CR(router->internal_error_handler(CRR(req, res)));
  } else if ((result->flags & NOT_FOUND_MASK) == NOT_FOUND_MASK) {
    res = CR(router->not_found_handler(CRR(req, res)));
  } else if ((result->flags & NOT_ALLOWED_MASK) == NOT_ALLOWED_MASK) {
    res = CR(router->method_not_allowed_handler(CRR(req, res)));
  } else {
    req->parameters = result->parameters;
    req->queries = result->queries;

    route_handler *h = (route_handler *)result->action->handler;

    array_t *mws = router->middlewares;
    if (has_elements(mws)) {
      res->done = invoke_chain(req, res, mws);
    }

    if (!res->done) {
      res = CR(h(CRR(req, res)));
    }
  }

  response_send(ctx, response_serialize(req, res));

  // TODO: free ht
  free(req);
  free(res);
  free(ctx);
  free(result);
}

void router_free(http_router *router) {
  free(router);
  // TODO: free sub-routers
}

http_router *router_register_sub(http_router *parent_router, router_attr *attr,
                                 const char *subpath) {
  router_internal *parent = (router_internal *)parent_router;

  if (!parent->sub_routers) {
    parent->sub_routers = ht_init(0);
  }

  router_internal *sub_router = (router_internal *)router_init(attr);
  ht_insert(parent->sub_routers, subpath, sub_router);

  return (http_router *)sub_router;
}
