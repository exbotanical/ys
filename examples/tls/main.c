#include <stdlib.h>

#include "libys.h"

#define PORT 9000

ys_response *handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, "Hello TLS!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);
  ys_router_register(router, "/", handler, YS_METHOD_GET);

  ys_server_attr *srv_attr = ys_server_attr_init_with(
      router, PORT, "./certs/localhost.pem", "./certs/localhost-key.pem");
  ys_server *server = ys_server_init(srv_attr);

  ys_server_start(server);

  return EXIT_SUCCESS;
}

// curl https://localhost:9000 -v
