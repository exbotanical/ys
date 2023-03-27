#include "libhttp.h"
#include "tap.c/tap.h"

static response* _h1(request* req, response* res) { return NULL; }
static response* _h2(request* req, response* res) { return NULL; }
static response* _h3(request* req, response* res) { return NULL; }

static const route_handler* h1 = _h1;
static const route_handler* h2 = _h2;
static const route_handler* h3 = _h3;

void assert_middleware(route_handler* mw, route_handler* h) {
  ok(mw == h, "handler address matches");
}

void test_middlewares_macro() {
  router_attr* attr = router_attr_init();
  middlewares(attr, h1, h2, h3);

  router_internal* r = (router_internal*)router_init(attr);

  ok(array_size(r->middlewares) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
  assert_middleware(array_get(r->middlewares, 1), _h2);
  assert_middleware(array_get(r->middlewares, 2), _h3);
}

void test_add_middleware() {
  router_attr* attr = router_attr_init();
  middlewares(attr, h1, h3);

  router_internal* r = (router_internal*)router_init(attr);

  ok(array_size(r->middlewares) == 2, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
  assert_middleware(array_get(r->middlewares, 1), _h3);

  add_middleware(attr, h2);

  ok(array_size(r->middlewares) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
  assert_middleware(array_get(r->middlewares, 1), _h3);
  assert_middleware(array_get(r->middlewares, 2), _h2);
}

void test_add_middleware_empty_attr() {
  router_attr* attr = router_attr_init();

  add_middleware(attr, h1);
  router_internal* r = (router_internal*)router_init(attr);

  ok(array_size(r->middlewares) == 1, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
}

void test_use_cors() {
  router_attr* attr = router_attr_init();

  use_cors(attr, cors_allow_all());
  router_internal* r = (router_internal*)router_init(attr);

  ok(r->use_cors == true, "sets use_cors flag to true");
  ok(array_size(r->middlewares) == 1, "use_cors sets CORS middleware");
}

int main() {
  plan(15);

  test_middlewares_macro();
  test_add_middleware();
  test_add_middleware_empty_attr();
  test_use_cors();

  done_testing();
}
