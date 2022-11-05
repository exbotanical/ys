#include "array.h"
#include "http.h"
#include "util.h"
#include "router.h"
#include "path.h"
#include "context.h"

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

typedef struct client_context {
	int client_socket;
	struct sockaddr *address;
	socklen_t *addr_len;
	router_t *router;
} client_context_t;

int main() {
	run();
	return EXIT_SUCCESS;
}

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

void client_thread_handler(void* args) {
	client_context_t *c_ctx = args;
		char recv_buffer[1024];

		recvfrom(
			c_ctx->client_socket,
			(char *)recv_buffer,
			sizeof(recv_buffer), 0,
			c_ctx->address,
			c_ctx->addr_len
		);

		printf("BUFFER: %s\n", recv_buffer);

		struct request r = build_request(recv_buffer);
		route_context_t *context = route_context_init(
			c_ctx->client_socket,
			r.method,
			r.url,
			NULL
		);

		router_run(c_ctx->router, context);
}

int run() {
	router_t *router = router_init(NULL, NULL);
	router_register(router, collect_methods("GET", NULL), PATH_ROOT, handler);

	int master_socket;
	int client_socket;
	fd_set readfds;

	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Create socket file descriptor
	if ((master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// memset(address.sin_zero, NULL_TERM, sizeof address.sin_zero);

	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(master_socket, 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Listening on port %i...\n", PORT);

  while(1) {
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);

		select(master_socket + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(master_socket, &readfds)) {
			if ((client_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			pthread_t client_thread;
			pthread_attr_t attr;

			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			client_context_t *c_ctx = malloc(sizeof(client_context_t));
			c_ctx->address = (struct sockaddr *)&address;
			c_ctx->addr_len = &addrlen;
			c_ctx->client_socket = client_socket;
			c_ctx->router = router;

			pthread_create(&client_thread, &attr, client_thread_handler, c_ctx); // TODO: join

			pthread_join(client_thread, NULL);
		}
	}

	return EXIT_SUCCESS;
}
