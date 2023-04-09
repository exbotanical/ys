# Basic CORS

```c
#include <stdlib.h>

#include "libys.h"

#define PORT 9000

response *handler(request *req, response *res) {
  res_set_header(res, "Content-Type", "text/plain");
  res_set_header(res, "X-Powered-By", "demo");

  res_set_body(res, "Hello World!");
  res_set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  use_cors(attr, cors_allow_all());

  http_router *router = router_init(attr);
  router_register(router, "/", handler, METHOD_GET, NULL);

  tcp_server *server = server_init(router, PORT);
  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}

/*
curl localhost:9000 -H 'Access-Control-Request-Method: DELETE' -H
'Access-Control-Request-Headers: X-Test-Header' -H 'Origin: test.com' -X
OPTIONS -v
*/
```
