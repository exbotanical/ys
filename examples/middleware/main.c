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
  // TODO: pass in request, response
  response_t *response = get_response();
  set_header(response, "Content-Type: text/plain");
  set_header(response, "X-Powered-By: demo");

  set_body(response, "Hello World!");
  set_status(response, OK);

  return response;
}

void *middleware1(void *arg) {
  route_context_t *context = arg;
  if (array_size(context->parameters)) {
    parameter_t *p = array_get(context->parameters, 0);
    if (strcmp(p->key, "key") == 0) {
      if (strcmp(p->value, "match") == 0) {
        printf("match!!!\n");
      }
    }
  }

  printf("middleware 1\n");

  return arg;
}

void *middleware2(void *arg) {
  route_context_t *context = arg;
  printf("middleware 2\n");
  return arg;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  router_register(router, "/:key[^\\d+$]", handler,
                  collect_middleware(middleware1), GET, NULL);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
