#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>

#include "http.h"
#include "libutil/libutil.h"
#include "router.h"

/**
 * @brief Maximum number of queued connections allowed for server.
 */
static const int MAX_CONN = 100;

static const int DEFAULT_NUM_THREADS = 4;

/**
 * @brief A server configuration object.
 */
typedef struct server {
  router_t *router;
  int port;
} server_t;

/**
 * @brief A context object for a client connection.
 */
typedef struct client_context {
  int client_socket;
  struct sockaddr *address;
  socklen_t *addr_len;
  router_t *router;
} client_context_t;

/**
 * @brief A response object; client handlers must return this
 * struct to be sent to the client.
 */
typedef struct response {
  /** HTTP status code - required */
  http_status_t status;
  /** HTTP headers - optional, but you should pass content-type if sending a
   * body */
  ch_array_t *headers;
  /** Response body - optional; Content-length header will be set for you */
  char *body;
} response_t;

/**
 * @brief Request metadata collected from an inbound client request.
 */
typedef struct request {
  char *path;
  char *method;
  char *protocol;
  char *host;
  char *user_agent;
  char *accept;
  char *content_len;
  char *content_type;
  char *content;
  char *raw;
} request_t;

/**
 * @brief Allocates the necessary memory for a `server_t`.
 *
 * @param router
 * @param port
 */
server_t *server_init(router_t *router, int port);

/**
 * @brief Listens for client connections and executes routing.
 *
 * @param server
 */
void server_start(server_t *server);

/**
 * @brief Allocates the necessary memory for a `response_t`.
 *
 * @return response_t*
 */
response_t *response_init();

/**
 * @brief Sends the user-provided response and closes the socket connection.
 * @internal
 *
 * @param socket
 * @param response_data
 */
void send_response(int socket, response_t *response_data);

#endif /* SERVER_H */
