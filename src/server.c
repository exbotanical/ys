

#include <stdio.h>       // for perror
#include <stdlib.h>      // for exit
#include <string.h>      // for strlen, strtok
#include <sys/select.h>  // for fd_set
#include <unistd.h>      // for write, close

#include "client.h"                // for client_context_t
#include "config.h"                // for server_config
#include "lib.thread/libthread.h"  // for thread pools
#include "libhttp.h"
#include "logger.h"
#include "request.h"  // for deserialize_req
#include "response.h"
#include "router.h"
#include "util.h"
#include "xmalloc.h"

/**
 * client_thread_handler handles client connections and executes the
 * user-defined router
 * @param arg
 * @return void*
 */
static void *client_thread_handler(void *arg) {
  client_context_t *c_ctx = arg;

  req_meta_t maybe_req = req_read_and_parse(c_ctx->client_socket);

  // TODO: test
  if (maybe_req.err.code < 0) {
    res_preempterr(c_ctx->client_socket, maybe_req.err.code);
    return NULL;
  }

  req_t *req = maybe_req.req;

  printlogf(LOG_INFO, "[server::%s] client request received: %s %s\n", __func__,
            req->method, req->path);

  router_run(c_ctx->router, c_ctx->client_socket, req);

  return NULL;
}

server_t *server_init(router_t *router, int port) {
  if (port == -1) {
    port = server_config.port_num;
  }

  __server_t *server = xmalloc(sizeof(__server_t));

  server->router = (__router_t *)router;
  server->port = port;

  return (server_t *)server;
}

bool server_start(server_t *server) {
  int port = ((__server_t *)server)->port;
  int master_socket;

  if ((master_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
    printlogf(LOG_INFO,
              "[server::%s] failed to initialize server socket on port %d\n",
              __func__, port);
    perror("socket");
    return false;
  }

  int yes = 1;
  // avoid the "Address already in use" error message
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
      -1) {
    printlogf(LOG_INFO, "[server::%s] failed to set sock opt\n", __func__);
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
    printlogf(LOG_INFO, "[server::%s] failed to bind server socket on %s:%d\n",
              __func__, address.sin_addr, port);
    perror("bind");
    return false;
  }

  if (listen(master_socket, MAX_CONN) < 0) {
    printlogf(LOG_INFO, "[server::%s] failed to listen on %s:%d\n", __func__,
              address.sin_addr, port);
    perror("listen");
    return false;
  }

  printlogf(LOG_INFO, "[server::%s] Listening on port %d...\n", __func__, port);
  fd_set readfds;
  int client_socket;

  thread_pool_t *pool = calloc(1, sizeof(thread_pool_t));
  if (!pool) {
    DIE(EXIT_FAILURE, "[server::%s] failed to initialized thread pool\n",
        __func__);
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
        printlogf(LOG_INFO,
                  "[server::%s] failed to accept client socket on %s:%d\n",
                  __func__, address.sin_addr, port);
        perror("accept");
        return false;
      }

      printlogf(LOG_DEBUG,
                "[server::%s] accepted connection from new client; spawning "
                "handler thread...\n",
                __func__);

      client_context_t *c_ctx = xmalloc(sizeof(client_context_t));
      c_ctx->address = (struct sockaddr *)&address;
      c_ctx->addr_len = &addr_len;
      c_ctx->client_socket = client_socket;
      c_ctx->router = ((__server_t *)server)->router;

      // TODO: UH, why are we blocking?
      if (!thread_pool_dispatch(pool, client_thread_handler, c_ctx, true)) {
        DIE(EXIT_FAILURE, "[server::%s] failed to dispatch thread from pool\n",
            __func__);
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
