#include <stdlib.h>

#include "libys.h"

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

  tcp_server *server = server_init(server_attr_init(router));
  server_start(server);

  return EXIT_SUCCESS;
}

// curl localhost:5000 -v
// curl localhost:5000/api -v
// curl localhost:5000/api/demo -v
