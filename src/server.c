#include "server.h"

#include "array.h"
#include "http.h"
#include "util.h"
#include "path.h"
#include "logger.h"

#include "lib.thread/libthread.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdarg.h>
#include <sys/select.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>

void send_response(int socket, buffer_t *response) {
	write(socket, response->state, response->len);
	buffer_free(response);
	close(socket);
}

/**
 * @brief TODO:
 *
 * @param args
 */
void* client_thread_handler(void* args) {
	client_context_t *c_ctx = args;
  char recv_buffer[1024];

  recvfrom(
    c_ctx->client_socket,
    (char *)recv_buffer,
    sizeof(recv_buffer),
    0,
    c_ctx->address,
    c_ctx->addr_len
  );

  LOG("[server::client_thread_handler] client request received: %s\n", recv_buffer);

  struct request r = build_request(recv_buffer);
  route_context_t *context = route_context_init(
    c_ctx->client_socket,
    r.method,
    r.url,
    NULL
  );

  router_run(c_ctx->router, context);

  return NULL;
}

server_t *server_init(router_t *router, int port) {
  server_t *server = malloc(sizeof(server_t));
  if (!server) {
    char *message = "failed to allocate server";
    LOG("%s\n", message);
    DIE(EXIT_FAILURE, "%s\n", message);
  }

  server->router = router;
  server->port = port;

  return server;
}

void server_start(server_t *server) {
  int port = server->port;
	int master_socket;

	if ((master_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
    char *message = fmt_str(
      "failed to initialize server socket on port %d",
      port
    );
    LOG("[server::start] %s\n", message);
		perror("socket");
    DIE(EXIT_FAILURE, "%s\n", message);
	}

  struct sockaddr_in address;
	socklen_t addr_len = sizeof(address);

	memset((char *)&address, NULL_TERM, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    char *message = fmt_str(
      "failed to bind server socket on %s:%d",
      address.sin_addr,
      port
    );
    LOG("[server::start] %s\n", message);
		perror("bind");
    DIE(EXIT_FAILURE, "%s\n", message);
	}

	if (listen(master_socket, MAX_CONN) < 0) {
    char *message = fmt_str(
      "failed to listen on %s:%d",
      address.sin_addr,
      port
    );
    LOG("[server::start] %s\n", message);
		perror("listen");
    DIE(EXIT_FAILURE, "%s\n", message);
	}

	printf("Listening on port %d...\n", port);

	fd_set readfds;
	int client_socket;

  thread_pool_t* pool = calloc(1, sizeof(thread_pool_t));
  if (!pool) {
    char *message = "failed to initialized thread pool";
    LOG("%s\n", message);
    DIE(EXIT_FAILURE, "%s\n", message);
  }

	thread_pool_init(pool);

  while(true) {
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);

		select(master_socket + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(master_socket, &readfds)) {
			if ((client_socket = accept(master_socket, (struct sockaddr *)&address, &addr_len)) < 0) {
        char *message = fmt_str(
          "failed to accept client socket on %s:%d",
          address.sin_addr,
          port
        );
        LOG("[server::start] %s\n", message);
        perror("accept");
        DIE(EXIT_FAILURE, "%s\n", message);
			}

      LOG("[server::start] %s\n", "accepted connection from new client; spawning handler thread...");

  	  thread_t* client_thread = thread_init(0, "client thread");
      // Make this a detached thread
      thread_set_attr(client_thread, false);
	    thread_pool_insert(pool, client_thread);

			client_context_t *c_ctx = malloc(sizeof(client_context_t));
			c_ctx->address = (struct sockaddr *)&address;
			c_ctx->addr_len = &addr_len;
			c_ctx->client_socket = client_socket;
			c_ctx->router = server->router;

      if (!thread_pool_dispatch(pool, client_thread_handler, c_ctx, true)) {
        LOG("[server::start] %s\n", "failed to dispatch thread from pool");
        DIE(EXIT_FAILURE, "%s\n", "failed to dispatch thread from pool");
      }
		}
	}

  // TODO: interrupt cancel, kill sig
}
