#include <stdarg.h>  // for variadic args functions

#include "cors.h"
#include "libhttp.h"
#include "xmalloc.h"

void add_middleware(middlewares_t *m, handler_t *h) {
  array_push((__middlewares_t *)m->middlewares, h);
}

// TODO: replace with an attr object (pthread_once?)
middlewares_t *__middlewares(handler_t *middleware, ...) {
  array_t *middlewares = array_init();
  if (!middlewares) {
    return NULL;
  }

  va_list args;
  va_start(args, middleware);

  while (middleware != NULL) {
    if (!array_push(middlewares, (void *)middleware)) {
      return NULL;
    }

    middleware = va_arg(args, handler_t *);
  }

  va_end(args);

  __middlewares_t *m = xmalloc(sizeof(__middlewares_t));
  m->middlewares = middlewares;

  return (middlewares_t *)m;
}

handler_t *use_cors(cors_opts_t *opts) {
  cors_init(opts);

  return cors_handler;
}
