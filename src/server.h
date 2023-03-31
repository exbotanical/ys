#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#include "router.h"

/**
 * A server configuration object that stores settings for the HTTP server
 */
typedef struct {
  int port;
  router_internal *router;
} server_internal;

#endif /* SERVER_H */
