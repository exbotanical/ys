#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#include "router.h"

#ifdef USE_TLS
#include <openssl/ssl.h>
#endif

/**
 * A server configuration object that stores settings for the HTTP server
 */
typedef struct {
  int port;
  router_internal *router;

  char *cert_path;
  char *key_path;
#ifdef USE_TLS
  SSL_CTX *sslctx;
#endif
} server_internal;

#endif /* SERVER_H */
