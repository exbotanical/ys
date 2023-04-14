#include "middleware.h"

#include <stdarg.h>

#include "cors.h"
#include "libys.h"
#include "logger.h"
#include "regexpr.h"
#include "xmalloc.h"

void __add_middleware_with_opts(router_attr *attr, route_handler *mw,
                                char *ignore_path, ...) {
  middleware_handler *mh = xmalloc(sizeof(middleware_handler));
  mh->handler = mw;

  if (ignore_path != NULL) {
    mh->ignore_paths = array_init();

    va_list args;
    va_start(args, ignore_path);

    while (ignore_path) {
      pcre *re = regex_compile(ignore_path);
      if (!re) {
        printlogf(YS_LOG_INFO,
                  "failed to compile regex %s; this path will be omitted from "
                  "ignore paths\n",
                  ignore_path);
      }
      array_push(mh->ignore_paths, re);
      ignore_path = va_arg(args, char *);
    }

    va_end(args);
  } else {
    mh->ignore_paths = NULL;
  }

  router_attr_internal *attr_internal = (router_attr_internal *)attr;

  if (!has_elements(attr_internal->middlewares)) {
    attr_internal->middlewares = array_init();
  }

  array_push(attr_internal->middlewares, mh);
}

void __middlewares(router_attr *attr, route_handler *mw, ...) {
  va_list args;
  va_start(args, mw);

  while (mw != NULL) {
    __add_middleware_with_opts(attr, mw, NULL);

    mw = va_arg(args, route_handler *);
  }

  va_end(args);
}

void add_middleware(router_attr *attr, route_handler *mw) {
  __add_middleware_with_opts(attr, mw, NULL);
}

void use_cors(router_attr *attr, cors_opts *opts) {
  ((router_attr_internal *)attr)->use_cors = true;
  cors_init((cors_opts_internal *)opts);
  // TODO: ensure CORS runs first for OPTIONS requests
  add_middleware(attr, cors_handler);
}
