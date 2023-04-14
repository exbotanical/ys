#ifndef SERVER_H
#define SERVER_H

#include <openssl/ssl.h>
#include <stdbool.h>

#include "router.h"

typedef struct {
  int port;
  bool use_https;
  router_internal *router;
  char *cert_path;
  char *key_path;
} server_attr_internal;

/**
 * A server configuration object that stores settings for the HTTP server
 */
typedef struct {
  int port;
  router_internal *router;

  char *cert_path;
  char *key_path;
  SSL_CTX *sslctx;
} server_internal;

#endif /* SERVER_H */
