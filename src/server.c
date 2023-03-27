

#include <stdio.h>       // for perror
#include <stdlib.h>      // for exit
#include <string.h>      // for strlen, strtok
#include <sys/select.h>  // for fd_set
#include <unistd.h>      // for write, close

#include "client.h"                // for client_context
#include "config.h"                // for server_conf
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
  client_context *ctx = arg;

  maybe_request maybe_req = req_read_and_parse(ctx->sockfd);

  // TODO: test + fix
  if (maybe_req.err.code == IO_ERR || maybe_req.err.code == PARSE_ERR ||
      maybe_req.err.code == REQ_TOO_LONG || maybe_req.err.code == DUP_HDR) {
    printlogf(LOG_INFO,
              "[server::%s] error parsing client request with error code %d. "
              "Pre-empting response with internal error handler\n",
              __func__, maybe_req.err.code);

    res_preempterr(ctx->sockfd, maybe_req.err.code);
    return NULL;
  }

  request *req = maybe_req.req;

  printlogf(LOG_INFO, "[server::%s] client request received: %s %s\n", __func__,
            req->method, req->path);

  router_run(ctx->router, ctx->sockfd, req);

  return NULL;
}

tcp_server *server_init(http_router *router, int port) {
  if (port == -1) {
    port = server_conf.port;
  }

  server_internal *server = xmalloc(sizeof(server_internal));

  server->router = (router_internal *)router;
  server->port = port;

  return (tcp_server *)server;
}

bool server_start(tcp_server *server) {
  int port = ((server_internal *)server)->port;
  int server_sockfd;

  if ((server_sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
    printlogf(LOG_INFO,
              "[server::%s] failed to initialize server socket on port %d\n",
              __func__, port);
    perror("socket");

    return false;
  }

  int yes = 1;
  // avoid the "Address already in use" error message
  if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
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

  if (bind(server_sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    printlogf(LOG_INFO, "[server::%s] failed to bind server socket on %s:%d\n",
              __func__, address.sin_addr, port);
    perror("bind");

    return false;
  }

  if (listen(server_sockfd, MAX_QUEUED_CONNECTIONS) < 0) {
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

  const int num_threads = server_conf.threads;
  thread_pool_init(pool);

  for (int i = 0; i < num_threads; i++) {
    thread_t *client_thread = thread_init(0, "client thread");

    // Make this a detached thread
    thread_set_attr(client_thread, false);
    thread_pool_insert(pool, client_thread);
  }

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);

    select(server_sockfd + 1, &readfds, NULL, NULL, NULL);

    if (FD_ISSET(server_sockfd, &readfds)) {
      if ((client_socket = accept(server_sockfd, (struct sockaddr *)&address,
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

      client_context *ctx = xmalloc(sizeof(client_context));
      ctx->sockfd = client_socket;
      ctx->router = ((server_internal *)server)->router;

      // TODO: UH, why are we blocking?
      if (!thread_pool_dispatch(pool, client_thread_handler, ctx, true)) {
        DIE(EXIT_FAILURE, "[server::%s] failed to dispatch thread from pool\n",
            __func__);
      }
    }
  }

  // TODO: interrupt cancel, kill sig
  return true;
}

void server_free(tcp_server *server) {
  router_free((http_router *)((server_internal *)server)->router);
  free(server);
}
