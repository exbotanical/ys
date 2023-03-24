#include <stdlib.h>

#include "libhttp.h"

res_t *handler(req_t *req, res_t *res) {
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr_t *attr = router_attr_init();
  router_t *router = router_init(attr);
  router_register(router, "/", handler, METHOD_GET, NULL);

  server_t *server = server_init(router, -1);
  if (!server_start(server)) {
    server_free(server);  // also frees router

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
