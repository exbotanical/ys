#include <stdarg.h>  // for variadic args functions

#include "cors.h"
#include "libhttp.h"
#include "xmalloc.h"

void add_middleware(router_attr_t *r, handler_t *mw) {
  if (!r->middlewares) r->middlewares = array_init();
  array_push(r->middlewares, mw);
}

void __middlewares(router_attr_t *r, handler_t *mw, ...) {
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

    mw = va_arg(args, handler_t *);
  }

  va_end(args);

  r->middlewares = mws;
}

void use_cors(router_attr_t *r, cors_opts_t *opts) {
  r->use_cors = true;
  cors_init(opts);

  if (!has_elements(r->middlewares)) {
    r->middlewares = array_init();
  }

  array_push(r->middlewares, cors_handler);
}
