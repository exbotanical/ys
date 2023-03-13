#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhttp.h"

#define PORT 9000

res_t *handler(req_t *req, res_t *res) {
  set_header(res, "Content-Type: text/plain");
  set_header(res, "X-Powered-By: demo");

  set_body(res, "Hello World!");
  set_status(res, OK);

  return res;
}

res_t *middleware1(req_t *req, res_t *res) {
  printf("middleware 1\n");

  return res;
}

res_t *middleware2(req_t *req, res_t *res) {
  printf("middleware 2\n");

  return res;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  router_register(router, "/:key[^\\d+$]", handler,
                  collect_middleware(middleware1, middleware2, NULL), GET,
                  NULL);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    server_free(server);  // also frees router

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
