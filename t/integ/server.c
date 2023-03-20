#include <stdlib.h>

#include "libhttp.h"

#define PORT 9000

res_t *handler(req_t *req, res_t *res) {
  set_header(res, "Content-Type: text/plain");
  set_header(res, "X-Powered-By: integ-test");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  router_register(router, "/", handler, NULL, METHOD_GET, NULL);

  // passing PORT explicitly overrides config value if there is one
  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    server_free(server);  // also frees router

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
