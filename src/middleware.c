#include <stdarg.h>  // for variadic args functions

#include "libhttp.h"

array_t *collect_middleware(res_t *(*middleware)(req_t *, res_t *), ...) {
  array_t *middlewares = array_init();
  if (!middlewares) {
    return NULL;
  }

  va_list args;
  va_start(args, middleware);

  while (middleware != NULL) {
    if (!array_push(middlewares, middleware)) {
      return NULL;
    }

    middleware = va_arg(args, void *(*)(void *));
  }

  va_end(args);

  return middlewares;
}
