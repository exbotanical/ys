#include <stdarg.h>  // for variadic args functions

#include "cors.h"
#include "libhttp.h"

void add_middleware(router_attr *attr, route_handler *mw) {
  if (!has_elements(attr->middlewares)) {
    attr->middlewares = array_init();
  }

  array_push(attr->middlewares, mw);
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

  attr->middlewares = mws;
}

void use_cors(router_attr *attr, cors_opts *opts) {
  attr->use_cors = true;
  cors_init((cors_opts_internal *)opts);

  if (!has_elements(attr->middlewares)) {
    attr->middlewares = array_init();
  }

  array_push(attr->middlewares, cors_handler);
}
