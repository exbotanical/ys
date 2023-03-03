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

#include "array.h"
#include "http.h"
#include "lib.thread/libthread.h"
#include "path.h"
#include "router.h"
#include "server.h"

#define PORT 9000

void *handler(void *arg) {
  route_context_t *context = arg;
  if (!context) {
    exit(EXIT_FAILURE);
  }

  response_t *response = response_init();
  if (!response) {
    exit(EXIT_FAILURE);
  }

  if (!ch_array_insert(response->headers, "Content-Type: text/plain") ||
      !ch_array_insert(response->headers, "X-Powered-By: demo")) {
    exit(EXIT_FAILURE);
  }

  response->body = "Hello World!";
  response->status = OK;

  return response;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  router_register(router, collect_methods("GET", NULL), PATH_ROOT, handler);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
