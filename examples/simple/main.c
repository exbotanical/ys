#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "lib.thread/libthread.h"
#include "libhttp.h"

#define PORT 9000

void *handler(void *arg) {
  Response *response = get_response();
  set_header(response, "Content-Type: text/plain");
  set_header(response, "X-Powered-By: demo");

  set_body(response, "Hello World!");
  set_status(response, OK);

  return response;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  router_register(router, "/", handler, GET, NULL);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
