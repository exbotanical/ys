#ifndef SERVER_H
#define SERVER_H

#include "router.h"

#include <sys/socket.h>

/**
 * @brief TODO:
 */
static const int MAX_CONN = 100;

/**
 * @brief TODO:
 */
typedef struct server {
  router_t *router;
  int port;
} server_t;

/**
 * @brief TODO:
 */
typedef struct client_context {
	int client_socket;
	struct sockaddr *address;
	socklen_t *addr_len;
	router_t *router;
} client_context_t;

typedef struct response {
  http_status_t status;
  char **headers;
  char *body;
} response_t;

/**
 * @brief TODO:
 *
 * @param router
 * @param port
 */
server_t *server_init(router_t *router, int port);

/**
 * @brief TODO:
 *
 * @param server
 */
void server_start(server_t *server);

void send_response(int socket, buffer_t *response);

#endif /* SERVER_H */
