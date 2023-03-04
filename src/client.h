#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>  // TODO: check all includes and comment

#include "router.h"

/**
 * @brief A context object to store metadata about a client socket connection.
 */
typedef struct client_context {
  int client_socket;
  struct sockaddr *address;
  socklen_t *addr_len;
  router_t *router;
} client_context_t;

#endif /* CLIENT_H */
