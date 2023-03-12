#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhttp.h"

#define PORT 9000

void *handler(void *arg) {
  // TODO: pass in request, response
  response_t *response = get_response();
  set_header(response, "Content-Type: text/plain");
  set_header(response, "X-Powered-By: demo");

  set_body(response, "Hello World!");
  set_status(response, OK);

  return response;
}

void *middleware1(void *arg) {
  printf("middleware 1\n");

  return arg;
}

void *middleware2(void *arg) {
  printf("middleware 2\n");

  return arg;
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
