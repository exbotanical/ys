#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>

#ifdef USE_TLS
#include <openssl/ssl.h>
#endif

/**
 * client_context is a context object to store metadata about a client socket
 * connection
 */
typedef struct {
#ifdef USE_TLS
  SSL* ssl;
#endif

  /**
   * The socket file descriptor on which the connection has been opened
   */
  int sockfd;
} client_context;

#endif /* CLIENT_H */
