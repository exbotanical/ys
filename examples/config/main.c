#include <stdlib.h>

#include "libhttp.h"

void *handler(void *arg) {
  response_t *response = get_response();
  set_header(response, "Content-Type: text/plain");
  set_header(response, "X-Powered-By: demo");

  set_body(response, "Hello World!");
  set_status(response, OK);

  return response;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  router_register(router, "/", handler, NULL, GET, NULL);

  server_t *server = server_init(router, -1);
  if (!server_start(server)) {
    server_free(server);  // also frees router

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
