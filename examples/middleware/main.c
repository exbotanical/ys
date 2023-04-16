#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libys.h"

response *handler(request *req, response *res) {
  printf("handler\n");

  char *key = req_get_parameter(req, "key");
  printf("key=%s\n", key);

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
  use_middlewares(attr, middleware1, middleware2);
  http_router *router = router_init(attr);

  router_register(router, "/:key[^\\d+$]", handler, METHOD_GET);

  tcp_server *server = server_init(server_attr_init(router));
  server_start(server);

  return EXIT_SUCCESS;
}
