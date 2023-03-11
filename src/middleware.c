#include <stdarg.h>

#include "libhttp.h"

array_t *collect_middleware(void *(*middleware)(void *), ...) {
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
