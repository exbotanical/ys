#include <stdio.h>
#include <stdlib.h>

#include "libhttp.h"
#define PORT 9000

res_t *handler(req_t *req, res_t *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "integ-test");
  set_header(res, "X-Not-Exposed", "integ-test");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

cors_opts_t *setup_cors() {
  cors_opts_t *c = malloc(sizeof(cors_opts_t));
  c->allowed_methods = array_collect("GET", "DELETE");
  c->allowed_headers = array_collect("X-Test-Header");
  c->expose_headers = array_collect("X-Powered-By");
  c->allowed_origins = array_collect("test.com");

  return c;
}

int main() {
  router_attr_t attr = ROUTE_ATTR_INITIALIZER;
  use_cors(&attr, setup_cors());
  router_t *router = router_init(attr);

  router_register(router, "/", handler, METHOD_GET, NULL);

  // passing PORT explicitly overrides config value if there is one
  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    server_free(server);  // also frees router

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
