#include <stdarg.h>  // for variadic args functions

#include "cors.h"
#include "libhttp.h"
#include "router.h"

void add_middleware(router_attr *attr, route_handler *mw) {
  router_attr_internal *attr_internal = (router_attr_internal *)attr;

  if (!has_elements(attr_internal->middlewares)) {
    attr_internal->middlewares = array_init();
  }

  array_push(attr_internal->middlewares, mw);
}

void __middlewares(router_attr *attr, route_handler *mw, ...) {
  array_t *mws = array_init();
  if (!mws) {
    return;
  }

  va_list args;
  va_start(args, mw);

  while (mw != NULL) {
    if (!array_push(mws, mw)) {
      return;
    }

    mw = va_arg(args, route_handler *);
  }

  va_end(args);

  ((router_attr_internal *)attr)->middlewares = mws;
}

void use_cors(router_attr *attr, cors_opts *opts) {
  router_attr_internal *attr_internal = (router_attr_internal *)attr;

  attr_internal->use_cors = true;
  cors_init((cors_opts_internal *)opts);

  if (!has_elements(attr_internal->middlewares)) {
    attr_internal->middlewares = array_init();
  }

  array_push(attr_internal->middlewares, cors_handler);
}
