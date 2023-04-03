#include <stdlib.h>

#include "libhttp.h"

#define PORT 9000

response *root_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "Root router");
  set_status(res, STATUS_OK);

  return res;
}

response *api_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "API root router");
  set_status(res, STATUS_OK);

  return res;
}

response *demo_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "API demo router");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);
  router_register(router, "/", root_handler, METHOD_GET, NULL);

  http_router *api_router = router_register_sub(router, attr, "/api");
  router_register(api_router, "/", api_handler, METHOD_GET, NULL);
  router_register(api_router, "/demo", demo_handler, METHOD_GET, NULL);

  // passing PORT explicitly overrides config value if there is one
  tcp_server *server = server_init(router, PORT);
  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}

// curl localhost:9000 -v
// curl localhost:9000/api -v
// curl localhost:9000/api/demo -v