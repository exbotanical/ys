#include "buffer.h"
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
	printf("MATCH! METHOD: %s, PATH: %s\n", context->method, context->path);

	buffer_t *response = build_response(
		OK,
		"text/plain",
		"Hello world!",
		"X-Powered-By: rest-c",
		NULL
	);

	write(context->client_socket, response->state, response->len);
	buffer_free(response);
	close(context->client_socket);

	return NULL;
}

int main() {
  router_t *router = router_init(NULL, NULL);
	router_register(router, collect_methods("GET", NULL), PATH_ROOT, handler);
	server_t *server = server_init(router, PORT);

  server_start(server);

	return EXIT_SUCCESS;
}
