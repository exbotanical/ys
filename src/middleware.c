#include <stdarg.h>  // for variadic args functions

#include "cors.h"
#include "libhttp.h"
#include "xmalloc.h"

void add_middleware(middlewares_t *mws, handler_t *mw) { array_push(mws, mw); }

void add_gmiddleware(router_t *r, handler_t *mw) {
  array_push(((__router_t *)r)->global_middlewares, mw);
}

middlewares_t *__middlewares(handler_t *mw, ...) {
  array_t *mws = array_init();
  if (!mws) {
    return NULL;
  }

  va_list args;
  va_start(args, mw);

  while (mw != NULL) {
    if (!array_push(mws, mw)) {
      return NULL;
    }

    mw = va_arg(args, handler_t *);
  }

  va_end(args);

  return mws;
}

void __gmiddlewares(router_t *r, handler_t *mw, ...) {
  array_t *mws = array_init();
  if (!mws) {
    return NULL;
  }

  va_list args;
  va_start(args, mw);

  while (mw != NULL) {
    if (!array_push(mws, mw)) {
      return NULL;
    }

    mw = va_arg(args, handler_t *);
  }

  va_end(args);

  ((__router_t *)r)->global_middlewares = mws;
}

handler_t *use_cors(cors_opts_t *opts) {
  cors_init(opts);

  return cors_handler;
}
