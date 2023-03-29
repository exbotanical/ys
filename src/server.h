#ifndef SERVER_H
#define SERVER_H

#include "router.h"

/**
 * A server configuration object that stores settings for the HTTP server
 */
typedef struct {
  router_internal *router;
  int port;
} server_internal;

#endif /* SERVER_H */
