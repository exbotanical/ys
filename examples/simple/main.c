#include "array.h"
#include "path.h"
#include "router.h"
#include "http.h"
#include "server.h"

#include "lib.thread/libthread.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdarg.h>
#include <sys/select.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>

#define PORT 9000

void *handler (void *arg) {
	route_context_t *context = arg;
  if (!context) {
    exit(EXIT_FAILURE);
  }

	printf("MATCH! METHOD: %s, PATH: %s\n", context->method, context->path);

  response_t *response = response_init();
  if (!response) {
    exit(EXIT_FAILURE);
  }

  if (
    !ch_array_insert(response->headers, "Content-Type: text/plain")
    || !ch_array_insert(response->headers, "X-Powered-By: demo")
  ) {
    exit(EXIT_FAILURE);
  }

  response->body = "Hello World!";
  response->status = OK;

	return response;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  if (!router) {
    return EXIT_FAILURE;
  }

	if (!router_register(router, collect_methods("GET", NULL), PATH_ROOT, handler)) {
    return EXIT_FAILURE;
  }

	server_t *server = server_init(router, PORT);
  server_start(server);

	return EXIT_SUCCESS;
}
