
#include "server.h"

#include <stdlib.h>      // for exit
#include <string.h>      // for strlen, strdup, strtok
#include <sys/select.h>  // for fd_set
#include <unistd.h>      // for write, close

#include "client.h"                // for client_context_t
#include "config.h"                // for server_config
#include "lib.thread/libthread.h"  // for thread pools
#include "libhttp.h"
#include "logger.h"
#include "request.h"  // for deserialize_req
#include "router.h"
#include "util.h"

/**
 * client_thread_handler handles client connections and executes the
 * user-defined router
 * @param arg
 * @return void*
 */
static void *client_thread_handler(void *arg) {
  client_context_t *c_ctx = arg;

  req_t *request = read_and_parse_request(c_ctx->client_socket);

  if (!request) {
    // TODO: somehow return a pre-prepared error flag so we can 500 this
    // we could preempt parsing into request so we dont waste computation
    return NULL;
  }

  LOG("[server::client_thread_handler] client request received: %s %s\n",
      request->method, request->path);

  router_run(c_ctx->router, c_ctx->client_socket, request);

  return NULL;
}

server_t *server_init(router_t *router, int port) {
  if (port == -1) {
    port = server_config.port_num;
  }

  __server_t *server = malloc(sizeof(__server_t));
  if (!server) {
    DIE(EXIT_FAILURE, "%s\n", "failed to allocate server");
  }

  server->router = (__router_t *)router;
  server->port = port;

  return (server_t *)server;
}

bool server_start(server_t *server) {
  int port = ((__server_t *)server)->port;
  int master_socket;

  if ((master_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
    LOG("[server::start] %s\n",
        fmt_str("failed to initialize server socket on port %d", port));
    perror("socket");
    return false;
  }

  int yes = 1;
  // avoid the "Address already in use" error message
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
      -1) {
    LOG("[server::start] %s\n", "failed to set sock opt");
    perror("setsockopt");
    return false;
  }

  struct sockaddr_in address;
  socklen_t addr_len = sizeof(address);

  memset((char *)&address, NULL_TERM, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    LOG("[server::start] %s\n", fmt_str("failed to bind server socket on %s:%d",
                                        address.sin_addr, port));
    perror("bind");
    return false;
  }

  if (listen(master_socket, MAX_CONN) < 0) {
    LOG("[server::start] %s\n",
        fmt_str("failed to listen on %s:%d", address.sin_addr, port));
    perror("listen");
    return false;
  }

  printf("Listening on port %d...\n", port);

  fd_set readfds;
  int client_socket;

  thread_pool_t *pool = calloc(1, sizeof(thread_pool_t));
  if (!pool) {
    DIE(EXIT_FAILURE, "%s\n", "failed to initialized thread pool");
  }

  const int num_threads = server_config.num_threads;
  thread_pool_init(pool);
  for (int i = 0; i < num_threads; i++) {
    thread_t *client_thread = thread_init(0, "client thread");
    // Make this a detached thread
    thread_set_attr(client_thread, false);
    thread_pool_insert(pool, client_thread);
  }

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(master_socket, &readfds);

    select(master_socket + 1, &readfds, NULL, NULL, NULL);
    if (FD_ISSET(master_socket, &readfds)) {
      if ((client_socket = accept(master_socket, (struct sockaddr *)&address,
                                  &addr_len)) < 0) {
        char *message = fmt_str("failed to accept client socket on %s:%d",
                                address.sin_addr, port);
        LOG("[server::start] %s\n", message);
        perror("accept");
        return false;
      }

      LOG("[server::start] %s\n",
          "accepted connection from new client; spawning handler thread...");

      client_context_t *c_ctx = malloc(sizeof(client_context_t));
      c_ctx->address = (struct sockaddr *)&address;
      c_ctx->addr_len = &addr_len;
      c_ctx->client_socket = client_socket;
      c_ctx->router = ((__server_t *)server)->router;

      if (!thread_pool_dispatch(pool, client_thread_handler, c_ctx, true)) {
        DIE(EXIT_FAILURE, "[server::start] %s\n",
            "failed to dispatch thread from pool");
      }
    }
  }

  // TODO: interrupt cancel, kill sig
  return true;
}

void server_free(server_t *server) {
  router_free((router_t *)((__server_t *)server)->router);
  free(server);
}
