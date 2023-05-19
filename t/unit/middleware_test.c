#include "middleware.h"

#include "libys.h"
#include "regexpr.h"
#include "router.h"
#include "tap.c/tap.h"
#include "tests.h"

static ys_response* h1(ys_request* req, ys_response* res) { return NULL; }
static ys_response* h2(ys_request* req, ys_response* res) { return NULL; }
static ys_response* h3(ys_request* req, ys_response* res) { return NULL; }

void assert_middleware(middleware_handler* mw, ys_route_handler* h) {
  ok(mw->handler == h, "handler address matches");
}

void test_middlewares_macro(void) {
  ys_router_attr* attr = ys_router_attr_init();
  ys_use_middlewares(attr, h1, h2, h3);

  router_internal* r = (router_internal*)ys_router_init(attr);

  ok(array_size(r->middlewares) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), h1);
  assert_middleware(array_get(r->middlewares, 1), h2);
  assert_middleware(array_get(r->middlewares, 2), h3);

  is(((middleware_handler*)array_get(r->middlewares, 0))->ignore_paths, NULL,
     "ignore paths initialized to NULL");
}

void test_add_middleware(void) {
  ys_router_attr* attr = ys_router_attr_init();
  ys_use_middlewares(attr, h1, h3);

  router_internal* r = (router_internal*)ys_router_init(attr);

  ok(array_size(r->middlewares) == 2, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), h1);
  assert_middleware(array_get(r->middlewares, 1), h3);
  ys_use_middleware(attr, h2);

  ok(array_size(r->middlewares) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), h1);
  assert_middleware(array_get(r->middlewares, 1), h3);
  assert_middleware(array_get(r->middlewares, 2), h2);

  is(((middleware_handler*)array_get(r->middlewares, 0))->ignore_paths, NULL,
     "ignore paths initialized to NULL");
}

void test_add_middleware_empty_attr(void) {
  ys_router_attr* attr = ys_router_attr_init();

  ys_use_middleware(attr, h1);
  router_internal* r = (router_internal*)ys_router_init(attr);

  ok(array_size(r->middlewares) == 1, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), h1);
}

void test_ys_use_cors(void) {
  ys_router_attr* attr = ys_router_attr_init();

  ys_use_cors(attr, ys_cors_allow_all());
  router_internal* r = (router_internal*)ys_router_init(attr);

  ok(r->use_cors == true, "sets use_cors flag to true");
  ok(array_size(r->middlewares) == 1, "ys_use_cors sets CORS middleware");

  is(((middleware_handler*)array_get(r->middlewares, 0))->ignore_paths, NULL,
     "ignore paths initialized to NULL");
}

void test_ys_add_middleware_with_opts_macro(void) {
  ys_router_attr* attr = ys_router_attr_init();

  ys_add_middleware_with_opts(attr, h2, "^/ignore1$", "^/ignore2$");

  router_internal* r = (router_internal*)ys_router_init(attr);

  middleware_handler* mh = ((middleware_handler*)array_get(r->middlewares, 0));
  isnt(mh->ignore_paths, NULL, "ignore paths initialized when paths specified");

  ok(array_size(mh->ignore_paths) == 2,
     "contains only the specified ignore paths");

  ok(regex_match(array_get(mh->ignore_paths, 0), "/ignore1") == true,
     "matches on the ignore path");
  ok(regex_match(array_get(mh->ignore_paths, 0), "/ignore") == false,
     "does not match on an unspecified path");

  ok(regex_match(array_get(mh->ignore_paths, 1), "/ignore2") == true,
     "matches on the ignore path");
  ok(regex_match(array_get(mh->ignore_paths, 1), "/") == false,
     "does not match on an unspecified path");

  assert_middleware(mh, h2);
}

void run_middleware_tests(void) {
  test_middlewares_macro();
  test_add_middleware();
  test_add_middleware_empty_attr();
  test_ys_use_cors();
  test_ys_add_middleware_with_opts_macro();
}
