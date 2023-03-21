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
  middlewares_t* mws = middlewares(h1, h2, h3);

  ok(array_size(mws) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(mws, 0), _h1);
  assert_middleware(array_get(mws, 1), _h2);
  assert_middleware(array_get(mws, 2), _h3);
}

void test_gmiddlewares_macro() {
  router_attr_t attr;
  __router_t* r = (__router_t*)router_init(attr);

  gmiddlewares(r, h1, h2, h3);

  ok(array_size(r->global_middlewares) == 3,
     "has the expected number of middlewares");
  assert_middleware(array_get(r->global_middlewares, 0), _h1);
  assert_middleware(array_get(r->global_middlewares, 1), _h2);
  assert_middleware(array_get(r->global_middlewares, 2), _h3);
}

void test_add_middleware() {
  middlewares_t* mws = middlewares(h1, h3);

  ok(array_size(mws) == 2, "has the expected number of middlewares");
  assert_middleware(array_get(mws, 0), _h1);
  assert_middleware(array_get(mws, 1), _h3);

  add_middleware(mws, h2);

  ok(array_size(mws) == 3, "has the expected number of middlewares");
  assert_middleware(array_get(mws, 0), _h1);
  assert_middleware(array_get(mws, 1), _h3);
  assert_middleware(array_get(mws, 2), _h2);
}

void test_add_gmiddleware() {
  router_attr_t attr;
  __router_t* r = (__router_t*)router_init(attr);

  gmiddlewares(r, h1, h3);

  ok(array_size(r->global_middlewares) == 2,
     "has the expected number of middlewares");
  assert_middleware(array_get(r->global_middlewares, 0), _h1);
  assert_middleware(array_get(r->global_middlewares, 1), _h3);

  add_gmiddleware((router_t*)r, h2);

  ok(array_size(r->global_middlewares) == 3,
     "has the expected number of middlewares");
  assert_middleware(array_get(r->global_middlewares, 0), _h1);
  assert_middleware(array_get(r->global_middlewares, 1), _h3);
  assert_middleware(array_get(r->global_middlewares, 2), _h2);
}

void test_use_cors() { printf("TODO:\n"); }

int main(int argc, char const* argv[]) {
  plan(22);

  test_middlewares_macro();
  test_gmiddlewares_macro();
  test_add_middleware();
  test_add_gmiddleware();
  test_use_cors();

  done_testing();
  return 0;
}
