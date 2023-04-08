# Simple Server

```c
#include <stdlib.h>

#include "libhttp.h"

#define PORT 9000

response *handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);
  router_register(router, "/", handler, METHOD_GET, NULL);

  // passing PORT explicitly overrides config value if there is one
  tcp_server *server = server_init(router, PORT);
  server_start(server);

  return EXIT_SUCCESS;
}

// curl localhost:9000 -v
```
