#include <stdio.h>
#include <stdlib.h>

#include "libhttp.h"
#define PORT 9000

res_t *handler(req_t *req, res_t *res) {
  res_setheader(res, "Content-Type", "text/plain");
  res_setheader(res, "X-Powered-By", "integ-test");
  res_setheader(res, "X-Not-Exposed", "integ-test");

  res_setbody(res, "Hello World!");
  res_setstatus(res, STATUS_OK);

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
  router_attr_t *attr = router_attr_init();
  use_cors(attr, setup_cors());
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
