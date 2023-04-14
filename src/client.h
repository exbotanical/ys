#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>

#include <openssl/ssl.h>

/**
 * client_context is a context object to store metadata about a client socket
 * connection
 */
typedef struct {
  SSL* ssl;

  /**
   * The socket file descriptor on which the connection has been opened
   */
  int sockfd;
} client_context;

#endif /* CLIENT_H */
