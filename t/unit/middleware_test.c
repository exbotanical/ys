#include "libhttp.h"
#include "tap.c/tap.h"

static res_t* _h1(req_t* req, res_t* res) { return NULL; }
static res_t* _h2(req_t* req, res_t* res) { return NULL; }
static res_t* _h3(req_t* req, res_t* res) { return NULL; }

static const handler_t* h1 = _h1;
static const handler_t* h2 = _h2;
static const handler_t* h3 = _h3;

void assert_middleware(handler_t* mw, handler_t* h) {
  ok(mw == h, "handler address matches");
}

void test_middlewares_macro() {
  router_attr_t attr;
  middlewares(&attr, h1, h2, h3);

  __router_t* r = (__router_t*)router_init(attr);

  ok(array_size(r->middlewares) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
  assert_middleware(array_get(r->middlewares, 1), _h2);
  assert_middleware(array_get(r->middlewares, 2), _h3);
}

void test_add_middleware() {
  router_attr_t attr;
  middlewares(&attr, h1, h3);

  __router_t* r = (__router_t*)router_init(attr);

  ok(array_size(r->middlewares) == 2, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
  assert_middleware(array_get(r->middlewares, 1), _h3);

  add_middleware(&attr, h2);

  ok(array_size(r->middlewares) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(r->middlewares, 0), _h1);
  assert_middleware(array_get(r->middlewares, 1), _h3);
  assert_middleware(array_get(r->middlewares, 2), _h2);
}

void test_use_cors() { printf("TODO:\n"); }

int main(int argc, char const* argv[]) {
  plan(11);

  test_middlewares_macro();
  test_add_middleware();
  test_use_cors();

  done_testing();
  return 0;
}
