#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>  // for socklen_t

#include "router.h"

/**
 * @brief A context object to store metadata about a client socket connection.
 */
typedef struct client_context {
  int client_socket;
  struct sockaddr *address;
  socklen_t *addr_len;
  // A pointer to the router so we can pass it into the client handler thread
  __router_t *router;
} client_context_t;

#endif /* CLIENT_H */
