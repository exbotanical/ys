#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhttp.h"

#define PORT 9000

response *handler(request *req, response *res) {
  printf("handler\n");

  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

response *middleware1(request *req, response *res) {
  printf("middleware 1\n");

  return res;
}

response *middleware2(request *req, response *res) {
  printf("middleware 2\n");

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  middlewares(attr, middleware1, middleware2);
  http_router *router = router_init(attr);

  router_register(router, "/:key[^\\d+$]", handler, METHOD_GET, NULL);

  tcp_server *server = server_init(router, PORT);
  if (!server_start(server)) {
    server_free(server);  // also frees router

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// curl localhost:9000/9 -v
