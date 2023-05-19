#include <stdlib.h>

#include "libys.h"

ys_response *handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);
  ys_router_register(router, "/", handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(ys_server_attr_init(router));
  ys_server_start(server);

  return EXIT_SUCCESS;
}
