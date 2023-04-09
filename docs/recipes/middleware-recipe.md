# Middleware

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libys.h"

#define PORT 9000

response *handler(request *req, response *res) {
  printf("handler\n");

  char *key = req_get_parameter(req, "key");
  printf("key=%s\n", key);

  res_set_header(res, "Content-Type", "text/plain");
  res_set_header(res, "X-Powered-By", "demo");

  res_set_body(res, "Hello World!");
  res_set_status(res, STATUS_OK);

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
  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}
```
