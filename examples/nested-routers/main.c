#include <stdlib.h>

#include "libys.h"

ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, "Root router");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *api_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, "API root router");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *demo_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, "API demo router");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);
  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  ys_router *api_router = ys_router_register_sub(router, attr, "/api");
  ys_router_register(api_router, "/", api_handler, YS_METHOD_GET);
  ys_router_register(api_router, "/demo", demo_handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(ys_server_attr_init(router));
  ys_server_start(server);

  return EXIT_SUCCESS;
}

// curl localhost:5000 -v
// curl localhost:5000/api -v
// curl localhost:5000/api/demo -v
