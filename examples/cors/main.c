#include <stdlib.h>

#include "libys.h"

response *handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  use_cors(attr, cors_allow_all());

  http_router *router = router_init(attr);
  router_register(router, "/", handler, METHOD_GET);

  tcp_server *server = server_init(server_attr_init(router));
  server_start(server);

  return EXIT_SUCCESS;
}

/*
curl localhost:5000 -H 'Access-Control-Request-Method: DELETE' -H
'Access-Control-Request-Headers: X-Test-Header' -H 'Origin: test.com' -X
OPTIONS -v
*/
