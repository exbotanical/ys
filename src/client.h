#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>  // for socklen_t

#include "router.h"

/**
 * client_context is a context object to store metadata about a client socket
 * connection
 */
typedef struct {
  /**
   * The socket file descriptor on which the connection has been opened
   */
  int sockfd;

  /**
   * A pointer to the router so we can pass it into the client handler thread
   */
  router_internal *router;
} client_context;

#endif /* CLIENT_H */
