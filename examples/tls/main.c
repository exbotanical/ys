#include <stdlib.h>

#include "libys.h"

#define PORT 9000

response *handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "Hello TLS!");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);
  router_register(router, "/", handler, METHOD_GET, NULL);

  tcp_server_attr *srv_attr = server_attr_init_with(
      router, PORT, "./certs/localhost.pem", "./certs/localhost-key.pem");
  tcp_server *server = server_init(srv_attr);

  server_start(server);

  return EXIT_SUCCESS;
}

// curl https://localhost:9000 -v
